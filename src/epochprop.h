/* Definitions for handling proper motions */

#include <postgres.h>


Datum		epoch_prop(PG_FUNCTION_ARGS);


/*
 * a cartesian point; this is like geo_decl's point, but you can't
 * have both geo_decls and pg_sphere right now (both define a type Point,
 * not to mention they have different ideas on EPSILON.
 */
typedef struct s_cpoint
{
	double		x,
				y;
} CPoint;

typedef struct s_phasevec
{
	SPoint		pos;			/* Position as an SPoint */
	double		pm[2];			/* Proper motion long/lat in rad/year, PM in
								 * longitude has cos(lat) applied */
	double		parallax;		/* in rad */
	double		rv;				/* radial velocity in km/s */
	int			parallax_valid; /* 1 if the parallax really is a NULL */
} phasevec;
