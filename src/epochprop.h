/* Definitions for handling proper motions */

#include <postgres.h>


extern Datum epoch_prop(PG_FUNCTION_ARGS);


typedef struct s_phasevec
{
	SPoint		pos;			/* Position as an SPoint */
	double		pm[2];			/* Proper motion long/lat in rad/year, PM in
								 * longitude has cos(lat) applied */
	double		parallax;		/* in rad */
	double		rv;				/* radial velocity in km/s */
	int			parallax_valid; /* 1 if we accept the parallax as physical */
} phasevec;
