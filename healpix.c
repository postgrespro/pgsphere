/* this goes in front to detect the chealpix API break */
#include "pgs_healpix.h"

#include <postgres.h>
#include <fmgr.h>
#include <catalog/pg_type.h>
#include "point.h" /* SPoint */
#include <math.h>

PG_FUNCTION_INFO_V1(pg_nest2ring);
PG_FUNCTION_INFO_V1(pg_ring2nest);
PG_FUNCTION_INFO_V1(healpix_convert_nest);
PG_FUNCTION_INFO_V1(healpix_convert_ring);
PG_FUNCTION_INFO_V1(pg_nside2order);
PG_FUNCTION_INFO_V1(pg_order2nside);
PG_FUNCTION_INFO_V1(pg_nside2npix);
PG_FUNCTION_INFO_V1(pg_npix2nside);
PG_FUNCTION_INFO_V1(healpix_nest);
PG_FUNCTION_INFO_V1(healpix_ring);
PG_FUNCTION_INFO_V1(inv_healpix_nest);
PG_FUNCTION_INFO_V1(inv_healpix_ring);

static int ilog2(hpint64 x)
{
	int log = 0;
	unsigned w;
	for (w = 32; w; w >>= 1)
	{
		hpint64 y = x >> w;
		if (y)
		{
			log += w;
			x = y;
		}
	}
	return log;
}

int
order_invalid(int order)
{
	return (order < 0 || order > 29);
}

static int nside_invalid(hpint64 nside)
{
	return (nside <= 0 || (nside - 1) & nside || order_invalid(ilog2(nside)));
}

static hpint64 c_nside(int order)
{
	hpint64 one_bit = 1;
	return one_bit << order;
}

hpint64 c_npix(int order)
{
	return nside2npix(c_nside(order));
}

static void check_order(int order)
{
	if (order_invalid(order))
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("Healpix level out of valid range [0..29]")));
}

static void check_index(int order, hpint64 i)
{
	check_order(order);
	if (i < 0 || i >= c_npix(order))
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("Healpix index out of range"),
				 errhint("Use nside2npix(order2nside(level)) to calculate"
					 " the respective limit\nfor Healpix indices.\n"
					 "Use healpix_convert(idx, from_level, to_level)"
					 " to move indices to another level.")));
}

static void check_nside(hpint64 nside)
{
	if (nside_invalid(nside))
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("nside value invalid"),
				 errhint("Valid nside values are only"
					 " order2nside(level),"
					 " for level in [0..29].")));
}

Datum pg_nest2ring(PG_FUNCTION_ARGS)
{
	int32 order  = PG_GETARG_INT32(0);
	hpint64 nest = PG_GETARG_INT64(1);
	check_index(order, nest);
	PG_RETURN_INT64(nest2ring(c_nside(order), nest));
}

Datum pg_ring2nest(PG_FUNCTION_ARGS)
{
	int32 order  = PG_GETARG_INT32(0);
	hpint64 ring = PG_GETARG_INT64(1);
	check_index(order, ring);
	PG_RETURN_INT64(ring2nest(c_nside(order), ring));
}

static hpint64 c_healpix_convert_nest(hpint64 idx, int32 from_order,
													int32 to_order)
{
	check_order(to_order);
	if (from_order > to_order)
		idx >>= (from_order - to_order) * 2;
	else
		idx <<= (to_order - from_order) * 2;
	return idx;
}

Datum healpix_convert_nest(PG_FUNCTION_ARGS)
{
	int32 to_order	 = PG_GETARG_INT32(0);
	int32 from_order = PG_GETARG_INT32(1);
	hpint64 nest	 = PG_GETARG_INT64(2);
	check_index(from_order, nest);
	PG_RETURN_INT64(c_healpix_convert_nest(nest, from_order, to_order));
}

Datum healpix_convert_ring(PG_FUNCTION_ARGS)
{
	int32 to_order	 = PG_GETARG_INT32(0);
	int32 from_order = PG_GETARG_INT32(1);
	hpint64 ring	 = PG_GETARG_INT64(2);
	hpint64 nest;
	check_index(from_order, ring);
	//nest = ring2nest(c_nside(from_order), ring);
	//nest = c_healpix_convert_nest(nest, from_order, to_order);
	nest = c_healpix_convert_nest(ring2nest(c_nside(from_order), ring), from_order, to_order);
	PG_RETURN_INT64(nest2ring(c_nside(to_order), nest));
}

Datum pg_nside2order(PG_FUNCTION_ARGS)
{
	hpint64 nside = PG_GETARG_INT64(0);
	check_nside(nside);
	PG_RETURN_INT32(ilog2(nside));
}

Datum pg_order2nside(PG_FUNCTION_ARGS)
{
	int32 order = PG_GETARG_INT32(0);
	check_order(order);
	PG_RETURN_INT64(c_nside(order));
}

Datum pg_nside2npix(PG_FUNCTION_ARGS)
{
	hpint64 nside = PG_GETARG_INT64(0);
	check_nside(nside);
	PG_RETURN_INT64(nside2npix(nside));
}

Datum pg_npix2nside(PG_FUNCTION_ARGS)
{
	hpint64 npix = PG_GETARG_INT64(0);
	hpint64 nside;
	if (npix < 12)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("npix value must be at least 12")));
	nside = floor(sqrt(npix * (1.0 / 12)) + 0.5);
	if (nside_invalid(nside) || nside2npix(nside) != npix)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("npix value invalid"),
				 errhint("Valid npix values are only"
					 " nside2npix(order2nside(level)),"
					 " for level in [0..29].")));
	PG_RETURN_INT64(nside);
}

static double conv_theta(double x)
{
	double y = PIH - x;
	if (fabs(x) < PI_EPS / 2)
		return PIH;
	if (fabs(y) < PI_EPS / 2)
		return 0;
	return y;
}

hpint64 healpix_nest_c(int32 order, SPoint* p)
{
	hpint64 i;
	t_ang angle = { conv_theta(p->lat), p->lng };
	i = ang2nest(c_nside(order), angle);
	return i;
}

Datum healpix_nest(PG_FUNCTION_ARGS)
{
	int32 order = PG_GETARG_INT32(0);
	SPoint* p = (SPoint*) PG_GETARG_POINTER(1);
	check_order(order);
	PG_RETURN_INT64(healpix_nest_c(order, p));
}

Datum healpix_ring(PG_FUNCTION_ARGS)
{
	int32 order = PG_GETARG_INT32(0);
	SPoint* p = (SPoint*) PG_GETARG_POINTER(1);
	t_ang angle = { conv_theta(p->lat), p->lng };
	check_order(order);
	PG_RETURN_INT64(ang2ring(c_nside(order), angle));
}

Datum inv_healpix_nest(PG_FUNCTION_ARGS)
{
	int32 order = PG_GETARG_INT32(0);
	hpint64 i = PG_GETARG_INT64(1);
	SPoint* p = (SPoint*) palloc(sizeof(SPoint));
	t_ang angle;
	check_index(order, i);
	angle = nest2ang(c_nside(order), i);
	p->lat = conv_theta(angle.theta);
	p->lng = angle.phi;
	PG_RETURN_POINTER(p);
}

Datum inv_healpix_ring(PG_FUNCTION_ARGS)
{
	int32 order = PG_GETARG_INT32(0);
	hpint64 i = PG_GETARG_INT64(1);
	SPoint* p = (SPoint*) palloc(sizeof(SPoint));
	t_ang angle;
	check_index(order, i);
	angle = ring2ang(c_nside(order), i);
	p->lat = conv_theta(angle.theta);
	p->lng = angle.phi;
	PG_RETURN_POINTER(p);
}
