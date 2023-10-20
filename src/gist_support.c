/**********************************************************************
 *
 * pgsphere gist_support.c
 * based on gserialized_supportfn.c from PostGIS
 *
 * PostGIS - Spatial Types for PostgreSQL
 * http://postgis.net
 *
 * PostGIS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * PostGIS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PostGIS.  If not, see <http://www.gnu.org/licenses/>.
 *
 **********************************************************************/


/* PostgreSQL */
#include "postgres.h"
#if PG_VERSION_NUM >= 120000
#include "funcapi.h"
#include "access/htup_details.h"
#include "access/stratnum.h"
#include "catalog/namespace.h"
#include "catalog/pg_opfamily.h"
#include "catalog/pg_type_d.h"
#include "catalog/pg_am_d.h"
#include "nodes/supportnodes.h"
#include "nodes/nodeFuncs.h"
#include "nodes/makefuncs.h"
#include "optimizer/optimizer.h"
#include "parser/parse_func.h"
#include "parser/parse_type.h"
#include "utils/array.h"
#include "utils/builtins.h"
#include "utils/lsyscache.h"
#include "utils/numeric.h"
#include "utils/selfuncs.h"
#include "utils/syscache.h"

#include "point.h"
#include "circle.h"

static Oid
scircleTypeOid(Oid callingfunc)
{
	/* type must be in same namespace as the caller */
	char *nspname = get_namespace_name(get_func_namespace(callingfunc));
	List *type_name_list = list_make2(makeString(nspname), makeString("scircle"));
	TypeName *type_name = makeTypeNameFromNameList(type_name_list);
	Oid type_oid = LookupTypeNameOid(NULL, type_name, true);
	if (type_oid == InvalidOid)
		elog(ERROR, "%s: unable to lookup type 'scircle'", __func__);
	return type_oid;
}

static Oid
scircleFunctionOid(Oid geotype, Oid callingfunc)
{
	const Oid radiustype = FLOAT8OID; /* Should always be FLOAT8OID */
	const Oid scircle_function_args[2] = {geotype, radiustype};
	const bool noError = true;
	/* Expand function must be in same namespace as the caller */
	char *nspname = get_namespace_name(get_func_namespace(callingfunc));
	List *scircle_function_name = list_make2(makeString(nspname), makeString("scircle"));
	Oid scircle_function_oid = LookupFuncName(scircle_function_name, 2, scircle_function_args, noError);
	if (scircle_function_oid == InvalidOid)
		elog(ERROR, "%s: unable to lookup 'scircle(Oid[%u], Oid[%u])'", __func__, geotype, radiustype);
	return scircle_function_oid;
}

PG_FUNCTION_INFO_V1(spherepoint_dwithin_supportfn);
Datum spherepoint_dwithin_supportfn(PG_FUNCTION_ARGS)
{
	Node *rawreq = (Node *) PG_GETARG_POINTER(0);
	Node *ret = NULL;

	if (IsA(rawreq, SupportRequestSelectivity))
	{
		SupportRequestSelectivity *req = (SupportRequestSelectivity *) rawreq;
		Node *radiusarg = (Node *) list_nth(req->args, 2);
		float8 selec;
		ereport(DEBUG1, (errmsg("spherepoint_dwithin_supportfn sel called on %d", req->funcid)));

		/*
		 * If the radius is a constant, compute the circle constant.
		 */
		if (IsA(radiusarg, Const))
		{
			Const	*constarg = (Const *) radiusarg;
			float8	 radius = DatumGetFloat8(constarg->constvalue);
			selec = spherecircle_area_float(radius) / SPHERE_SURFACE;
			ereport(DEBUG1, (errmsg("spherepoint_dwithin_supportfn const radius %g", radius)));
		}
		else
		{
			selec = DEFAULT_SCIRCLE_SEL;
			ereport(DEBUG1, (errmsg("spherepoint_dwithin_supportfn non-const radius")));
		}

		if (req->is_join)
		{
			req->selectivity = selec;
		}
		else
		{
			req->selectivity = selec;
		}
		CLAMP_PROBABILITY(req->selectivity);
		ereport(DEBUG1, (errmsg("spherepoint_dwithin_supportfn selectivity %g is_join %d", req->selectivity, req->is_join)));
		ret = rawreq;
	}

	else if (IsA(rawreq, SupportRequestIndexCondition))
	{
		SupportRequestIndexCondition *req = (SupportRequestIndexCondition *) rawreq;

		FuncExpr *clause = (FuncExpr *) req->node;
		Oid funcid = clause->funcid;
		Oid opfamilyoid = req->opfamily; /* OPERATOR FAMILY of the index */

		Node *leftarg, *rightarg, *radiusarg;
		Oid leftdatatype, oproid;

		Oid scircle_type_oid = scircleTypeOid(funcid);
		Expr *scircle_expr;
		Expr *expr;

		/*
		 * Extract "leftarg" as the arg matching the index and "rightarg" as
		 * the other, even if they were in the opposite order in the call.
		 */
		if (req->indexarg == 0)
		{
			leftarg = linitial(clause->args);
			rightarg = lsecond(clause->args);
		}
		else if (req->indexarg == 1)
		{
			rightarg = linitial(clause->args);
			leftarg = lsecond(clause->args);
		}
		else
			PG_RETURN_POINTER((Node *)NULL);

		leftdatatype = exprType(leftarg);
		Assert(leftdatatype == exprType(rightarg)); /* expect spoint, spoint */

		radiusarg = (Node *) list_nth(clause->args, 2);

		/*
		 * Given the index operator family and the arguments and the desired
		 * strategy number we can now lookup the operator we want.
		 */
		oproid = get_opfamily_member(opfamilyoid,
				leftdatatype,
				scircle_type_oid,
				37); /* spoint <@ scircle */
		if (!OidIsValid(oproid))
					elog(ERROR,
					     "no spatial operator found for '%s': opfamily %u types %d %d strategy %d",
					     "scircle",
					     opfamilyoid,
					     leftdatatype,
						 scircle_type_oid,
						 37);

		/*
		 * If both the right argument and the radius are a constant, compute
		 * the circle constant. (makeFuncExpr won't constify by itself
		 * unfortunately.)
		 */
		if (IsA(rightarg, Const) && IsA(radiusarg, Const))
		{
			Datum	 center = ((Const *) rightarg)->constvalue;
			Datum	 radius = ((Const *) radiusarg)->constvalue;
			Datum	 circle = DirectFunctionCall2(spherecircle_by_center, center, radius);
			scircle_expr = (Expr *) makeConst(scircle_type_oid, -1, InvalidOid,
					sizeof(SCIRCLE), circle, false, false);
			ereport(DEBUG1, (errmsg("spherepoint_dwithin_supportfn index condition const")));
		}
		else
		{
			Oid scircle_function_oid = scircleFunctionOid(leftdatatype, clause->funcid);
			scircle_expr = (Expr *) makeFuncExpr(scircle_function_oid, leftdatatype,
				list_make2(rightarg, radiusarg),
				InvalidOid, InvalidOid, COERCE_EXPLICIT_CALL);
			ereport(DEBUG1, (errmsg("spherepoint_dwithin_supportfn index condition function")));
		}

		/*
		* The comparison expression has to be a pseudo constant,
		* (not volatile or dependent on the target index table)
		*/
#if PG_VERSION_NUM >= 140000
		if (!is_pseudo_constant_for_index(req->root, (Node*)scircle_expr, req->index))
#else
		if (!is_pseudo_constant_for_index((Node*)scircle_expr, req->index))
#endif
			PG_RETURN_POINTER((Node*)NULL);

		/* OK, we can make an index expression */
		expr = make_opclause(oproid, BOOLOID, false,
					  (Expr *) leftarg, scircle_expr,
					  InvalidOid, InvalidOid);

		ret = (Node *)(list_make1(expr));

		/* This is an exact index lookup */
		req->lossy = false;
	}

	PG_RETURN_POINTER(ret);
}

#endif /* PG_VERSION_NUM */
