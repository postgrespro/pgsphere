#ifndef __GNOMO_H__
#define __GNOMO_H__

/* function prototypes for the direct and inverse gnomonic projections */

extern Datum gnomonic_proj(PG_FUNCTION_ARGS);
extern Datum gnomonic_inv(PG_FUNCTION_ARGS);

#endif
