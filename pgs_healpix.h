#ifndef __PGS_HEALPIX_H__
#define __PGS_HEALPIX_H__

/* this goes in front to detect the chealpix API break */
#include "pgs_chealpix.h"

#include <postgres.h>
#include <fmgr.h>

#include "point.h" /* SPoint */

int order_invalid(int);
hpint64 c_npix(int);
void check_order(int order);

/* function prototypes for the Healpix support functions */

Datum pg_nest2ring(PG_FUNCTION_ARGS);
Datum pg_ring2nest(PG_FUNCTION_ARGS);
Datum healpix_convert_nest(PG_FUNCTION_ARGS);
Datum healpix_convert_ring(PG_FUNCTION_ARGS);
Datum pg_nside2order(PG_FUNCTION_ARGS);
Datum pg_order2nside(PG_FUNCTION_ARGS);
Datum pg_nside2npix(PG_FUNCTION_ARGS);
Datum pg_npix2nside(PG_FUNCTION_ARGS);
Datum healpix_nest(PG_FUNCTION_ARGS);
Datum healpix_ring(PG_FUNCTION_ARGS);
Datum inv_healpix_nest(PG_FUNCTION_ARGS);
Datum inv_healpix_ring(PG_FUNCTION_ARGS);

hpint64 healpix_nest_c(int32, SPoint*);

#endif
