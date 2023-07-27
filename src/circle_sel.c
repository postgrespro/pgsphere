#include "circle.h"
#include <utils/selfuncs.h>
#include <utils/lsyscache.h>

/* Circle selectivity functions */

PG_FUNCTION_INFO_V1(spherepoint_in_circle_sel);
PG_FUNCTION_INFO_V1(spherepoint_in_circle_joinsel);
PG_FUNCTION_INFO_V1(spherepoint_in_circle_neg_sel);
PG_FUNCTION_INFO_V1(spherepoint_in_circle_neg_joinsel);
PG_FUNCTION_INFO_V1(spherepoint_in_circle_com_sel);
PG_FUNCTION_INFO_V1(spherepoint_in_circle_com_joinsel);
PG_FUNCTION_INFO_V1(spherepoint_in_circle_com_neg_sel);
PG_FUNCTION_INFO_V1(spherepoint_in_circle_com_neg_joinsel);

/*
 * Common code for spherepoint_in_circle_sel()
 */
static double
spherepoint_in_circle_sel_funcexpr(Node *other)
{
	FuncExpr *ofunc = (FuncExpr *)other;
	char *func_name = get_func_name(ofunc->funcid);
	Const *arg2;
	double radius;
	double selec;

	/*
	 * Safety checks: are we called on a function called scircle that takes a
	 * const double as 2nd argument?
	 */
	if (strcmp(func_name, "scircle") != 0)
	{
		ereport(DEBUG1, (errmsg("<@ selectivity called on function that is not scircle")));
		return DEFAULT_SCIRCLE_SEL;
	}
	if (list_length(ofunc->args) != 2)
	{
		ereport(DEBUG1, (errmsg("<@ selectivity called on function that does not have 2 arguments")));
		return DEFAULT_SCIRCLE_SEL;
	}
	if (!IsA(lsecond(ofunc->args), Const))
	{
		ereport(DEBUG1, (errmsg("<@ selectivity called on scircle() with non-const 2nd arg")));
		return DEFAULT_SCIRCLE_SEL;
	}
	arg2 = (Const *) lsecond(ofunc->args);
	if (arg2->consttype != FLOAT8OID)
	{
		ereport(DEBUG1, (errmsg("<@ selectivity called on scircle() with non-float8 2nd arg")));
		return DEFAULT_SCIRCLE_SEL;
	}

	radius = DatumGetFloat8(arg2->constvalue);
	selec = spherecircle_area_float(radius) / SPHERE_SURFACE;
	CLAMP_PROBABILITY(selec);

	return selec;
}

static double
spherepoint_in_circle_sel_internal(PG_FUNCTION_ARGS, bool commute, bool negate)
{
	PlannerInfo *root = (PlannerInfo *) PG_GETARG_POINTER(0);
	Oid         operator = PG_GETARG_OID(1);
	List       *args = (List *) PG_GETARG_POINTER(2);
	int         varRelid = PG_GETARG_INT32(3);
	//Oid         collation = PG_GET_COLLATION();
	VariableStatData vardata;
	Node       *other;
	bool        varonleft;
	double      selec;

	/*
	 * When asked about <>, we do the estimation using the corresponding =
	 * operator, then convert to <> via "1.0 - eq_selectivity - nullfrac".
	 */
	if (negate)
	{
		operator = get_negator(operator);
		if (!OidIsValid(operator))
		{
			/* Use default selectivity (should we raise an error instead?) */
			return 1.0 - DEFAULT_SCIRCLE_SEL;
		}
	}

	/*
	 * If expression is not variable = something or something = variable, then
	 * punt and return a default estimate.
	 */
	if (!get_restriction_variable(root, args, varRelid,
								  &vardata, &other, &varonleft))
		return negate ? (1.0 - DEFAULT_SCIRCLE_SEL) : DEFAULT_SCIRCLE_SEL;

	/*
	 * We can do a lot better if the something is a constant.  (Note: the
	 * Const might result from estimation rather than being a simple constant
	 * in the query.)
	 * Look only at var op circle_const, not var op point_const.
	 */
	if (IsA(other, Const) && varonleft != commute)
	{
		Const	*constnode = (Const *) other;
		SCIRCLE	*c;

		Assert(!constnode->constisnull); /* operators are strict, shouldn't have NULLs here */
		c = (SCIRCLE *) constnode->constvalue;
		selec = spherecircle_area_float(c->radius) / SPHERE_SURFACE;
		CLAMP_PROBABILITY(selec);
	}
	/*
	 * Check for <@ scircle(..., radius)
	 */
	else if (IsA(other, FuncExpr) && varonleft != commute)
	{
		selec = spherepoint_in_circle_sel_funcexpr(other);
		// p *((double * )&((*(Const *)((((FuncExpr*) other)->args->elements)[1].ptr_value)).constvalue))
	}
	else
	{
		ereport(DEBUG1, (errmsg("<@ selectivity not const, other node tag %i", other->type)));
		selec = DEFAULT_SCIRCLE_SEL;
	}

	ReleaseVariableStats(vardata);

	return negate ? (1.0 - selec) : selec;
}

/*
 * Common code for spherepoint_in_circle_joinsel()
 */
static double
spherepoint_in_circle_joinsel_internal(PG_FUNCTION_ARGS, bool commute)
{
#ifdef NOT_USED
	PlannerInfo *root = (PlannerInfo *) PG_GETARG_POINTER(0);
	Oid			operator = PG_GETARG_OID(1);
	List	   *args = (List *) PG_GETARG_POINTER(2);

	JoinType	jointype = (JoinType) PG_GETARG_INT16(3);
	SpecialJoinInfo *sjinfo = (SpecialJoinInfo *) PG_GETARG_POINTER(4);
	Oid			collation = PG_GET_COLLATION();
	double		selec;
	double		selec_inner;
	VariableStatData vardata1;
	VariableStatData vardata2;
	double		nd1;
	double		nd2;
	bool		isdefault1;
	bool		isdefault2;
	Oid			opfuncoid;
	AttStatsSlot sslot1;
	AttStatsSlot sslot2;
	bool		join_is_reversed;
	RelOptInfo *inner_rel;

	get_join_variables(root, args, sjinfo,
					   &vardata1, &vardata2, &join_is_reversed);

	ReleaseVariableStats(vardata1);
	ReleaseVariableStats(vardata2);
#endif

	/* return a constant default for now; practically joins should use the
	 * spoint_dwithin function instead which has join support with the
	 * additional advantage that it's symmetric */
	return DEFAULT_SCIRCLE_SEL;
}

Datum
spherepoint_in_circle_sel(PG_FUNCTION_ARGS)
{
	PG_RETURN_FLOAT8(spherepoint_in_circle_sel_internal(fcinfo, false, false));
}

Datum
spherepoint_in_circle_joinsel(PG_FUNCTION_ARGS)
{
	PG_RETURN_FLOAT8(spherepoint_in_circle_joinsel_internal(fcinfo, false));
}

Datum
spherepoint_in_circle_neg_sel(PG_FUNCTION_ARGS)
{
	PG_RETURN_FLOAT8(spherepoint_in_circle_sel_internal(fcinfo, false, true));
}

Datum
spherepoint_in_circle_neg_joinsel(PG_FUNCTION_ARGS)
{
	PG_RETURN_FLOAT8(DEFAULT_INEQ_SEL);
}

Datum
spherepoint_in_circle_com_sel(PG_FUNCTION_ARGS)
{
	PG_RETURN_FLOAT8(spherepoint_in_circle_sel_internal(fcinfo, true, false));
}

Datum
spherepoint_in_circle_com_joinsel(PG_FUNCTION_ARGS)
{
	PG_RETURN_FLOAT8(spherepoint_in_circle_joinsel_internal(fcinfo, true));
}

Datum
spherepoint_in_circle_com_neg_sel(PG_FUNCTION_ARGS)
{
	PG_RETURN_FLOAT8(spherepoint_in_circle_sel_internal(fcinfo, true, true));
}

Datum
spherepoint_in_circle_com_neg_joinsel(PG_FUNCTION_ARGS)
{
	PG_RETURN_FLOAT8(DEFAULT_INEQ_SEL);
}
