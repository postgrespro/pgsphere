#include "types.h"

#if PG_VERSION_NUM >= 120000
#include "utils/float.h"
#include "common/shortest_dec.h"
#endif

#include <limits.h>

#if !defined(PGSPHERE_VERSION)
#error "PGSPHERE_VERSION macro is not set"
#endif

#define PGSPHERE_STRINGIFY_INTERNAL(x) #x
#define PGSPHERE_STRINGIFY(x) PGSPHERE_STRINGIFY_INTERNAL(x)

/* Output functions */

/* Output modes */
#define OUTPUT_RAD	1	 /* output in radians */
#define OUTPUT_DEG	2	 /* output in degrees */
#define OUTPUT_DMS	3	 /* output in degrees, minutes, seconds */
#define OUTPUT_HMS	4	 /* output in hour, minutes, seconds */

/*.
 * Holds the current output modus.
 * Default is radians.
 */
static unsigned char sphere_output = OUTPUT_RAD;

/*
 * Defines the precision of floating point values in output.
 */
static int sphere_output_precision = INT_MAX;

PG_FUNCTION_INFO_V1(set_sphere_output);
PG_FUNCTION_INFO_V1(spherepoint_out);
PG_FUNCTION_INFO_V1(spherecircle_out);
PG_FUNCTION_INFO_V1(sphereline_out);
PG_FUNCTION_INFO_V1(spheretrans_out);
PG_FUNCTION_INFO_V1(spherepoly_out);
PG_FUNCTION_INFO_V1(spherepath_out);
PG_FUNCTION_INFO_V1(sphereellipse_out);
PG_FUNCTION_INFO_V1(spherebox_out);
PG_FUNCTION_INFO_V1(set_sphere_output_precision);
PG_FUNCTION_INFO_V1(reset_sphere_output_precision);
PG_FUNCTION_INFO_V1(pg_sphere_version);

 /*
  * Sets the output modus.
  */
Datum	set_sphere_output(PG_FUNCTION_ARGS);

 /*
  * Sets the output precision.
  */
Datum	set_sphere_output_precision(PG_FUNCTION_ARGS);

 /*
  * The output function of spherical point.
  */
Datum	spherepoint_out(PG_FUNCTION_ARGS);

 /*
  * The output function of spherical circle.
  */
Datum	spherecircle_out(PG_FUNCTION_ARGS);

 /*
  * The output function of spherical line.
  */
Datum	sphereline_out(PG_FUNCTION_ARGS);

 /*
  * The output function of Euler transformation.
  */
Datum	spheretrans_out(PG_FUNCTION_ARGS);

 /*
  * The output function of spherical polygon.
  */
Datum	spherepoly_out(PG_FUNCTION_ARGS);

 /*
  * The output function of spherical path.
  */
Datum	spherepath_out(PG_FUNCTION_ARGS);

 /*
  * The output function of spherical ellipse.
  */
Datum	sphereellipse_out(PG_FUNCTION_ARGS);

 /*
  * The output function of spherical box.
  */
Datum	spherebox_out(PG_FUNCTION_ARGS);

 /*
  * Returns pg_sphere version.
  */
Datum	pg_sphere_version(PG_FUNCTION_ARGS);

static void
spheretrans_out_buffer(StringInfo si, const SEuler *se);

 /*
  * Converts radians to DEG ( degrees, minutes, seconds )
  *
  * rad - input in radians
  * deg - pointer to degrees
  * min - pointer to minutes
  * sec - pointer to seconds
  */
static void
rad_to_dms(double rad, unsigned int *deg, unsigned int *min, double *sec)
{
	double			rsign = rad < 0 ? -1 : 1;
	const double	sec_dig = pow(10, FLT_DIG);

	rad *= (rsign * RADIANS);

	*deg = floor(rad);
	*min = floor(60 * (rad - *deg));
	*sec = rad * 3600 - *min * 60 - *deg * 3600;
	*sec = rint((double) (*sec * sec_dig)) / sec_dig;

	if (*sec >= 60.0)
	{
		*sec -= 60.0;
		(*min)++;
	}
	if (*min >= 60)
	{
		*min -= 60;
		(*deg)++;
	}
}

static void
pgs_strinfo_put_chr(StringInfo si, char c)
{
	appendStringInfoChar(si, c);
}

static void
pgs_strinfo_put_str(StringInfo si, char *s)
{
	appendStringInfoString(si, s);
}

static void
pgs_strinfo_put_d64(StringInfo si, double value)
{
	int			cw,
				ndig;
	char		buf[128];

#if PG_VERSION_NUM >= 120000

	if (extra_float_digits > 0)
	{
		cw = double_to_shortest_decimal_buf(value, buf);
	}
	else
	{
		ndig = DBL_DIG + extra_float_digits;
		if (ndig < 1)
			ndig = 1;

		cw = pg_strfromd(buf, 128, ndig, value);
	}

#else

	ndig = DBL_DIG + extra_float_digits;
	if (ndig < 1)
		ndig = 1;

	cw = snprintf(buf, 128, "%.*g", ndig, value);

#endif

	if (cw < 0)
	{
		fflush(stderr);
		abort();
	}

	pgs_strinfo_put_str(si, buf);
}

static void
pgs_strinfo_put_lng_dms(StringInfo si, double lng)
{
	unsigned int	lngdeg,
					lngmin;
	double			lngsec;

	rad_to_dms(lng, &lngdeg, &lngmin, &lngsec);

	appendStringInfo(si, "%3ud %2um ", lngdeg, lngmin);
	pgs_strinfo_put_d64(si, lngsec);
	pgs_strinfo_put_chr(si, 's');
}

static void
pgs_strinfo_put_lng_hms(StringInfo si, double lng)
{
	unsigned int	lnghour,
					lngmin;
	double			lngsec;

	rad_to_dms(lng / 15.0, &lnghour, &lngmin, &lngsec);

	appendStringInfo(si, "%3uh %2um ", lnghour, lngmin);
	pgs_strinfo_put_d64(si, lngsec);
	pgs_strinfo_put_chr(si, 's');
}

static void
pgs_strinfo_put_lat_dms(StringInfo si, double lat)
{
	unsigned int	latdeg,
					latmin;
	double			latsec;
	const char		latsign = lat >= 0 ? '+' : '-';

	rad_to_dms(lat, &latdeg, &latmin, &latsec);

	appendStringInfo(si, "%c%2ud %2um ", latsign, latdeg, latmin);
	pgs_strinfo_put_d64(si, latsec);
	pgs_strinfo_put_chr(si, 's');
}

static void
pgs_strinfo_put_radius_dms(StringInfo si, double radius)
{
	unsigned int	rdeg,
					rmin;
	double			rsec;

	rad_to_dms(radius, &rdeg, &rmin, &rsec);

	appendStringInfo(si, "%2ud %2um ", rdeg, rmin);
	pgs_strinfo_put_d64(si, rsec);
	pgs_strinfo_put_chr(si, 's');
}

Datum
set_sphere_output_precision(PG_FUNCTION_ARGS)
{
	short int	c = PG_GETARG_INT16(0);
	char	   *buf = (char *) palloc(20);

	if (c > DBL_DIG)
		c = DBL_DIG;
	if (c < 1)
		c = DBL_DIG;
	sphere_output_precision = c;

	sprintf(buf, "SET %d", c);
	PG_RETURN_CSTRING(buf);
}

Datum
reset_sphere_output_precision(PG_FUNCTION_ARGS)
{
	char	   *buf = (char *) palloc(20);

	sphere_output_precision = INT_MAX;

	sprintf(buf, "RESET");

	PG_RETURN_CSTRING(buf);
}

Datum
set_sphere_output(PG_FUNCTION_ARGS)
{
	char	   *c = PG_GETARG_CSTRING(0);
	char	   *buf = (char *) palloc(20);

	if (strcmp(c, "RAD") == 0)
	{
		sphere_output = OUTPUT_RAD;
	}
	else if (strcmp(c, "DEG") == 0)
	{
		sphere_output = OUTPUT_DEG;
	}
	else if (strcmp(c, "DMS") == 0)
	{
		sphere_output = OUTPUT_DMS;
	}
	else if (strcmp(c, "HMS") == 0)
	{
		sphere_output = OUTPUT_HMS;
	}
	else
	{
		elog(ERROR, "Unknown format");
		pfree(buf);
		PG_RETURN_NULL();
	}
	sprintf(buf, "SET %s", c);
	PG_RETURN_CSTRING(buf);
}

static Datum
spherepoint_out_compat(PG_FUNCTION_ARGS)
{
	SPoint		   *sp = (SPoint *) PG_GETARG_POINTER(0);
	char		   *buffer = (char *) palloc(255);
	unsigned int	latdeg,
					latmin,
					lngdeg,
					lngmin;
	double			latsec,
					lngsec;

	latdeg = latmin = lngdeg = lngmin = 0;
	latsec = lngsec = 0.0;

	switch (sphere_output)
	{
		case OUTPUT_DEG:
			sprintf(buffer, "(%.*gd , %.*gd)",
					sphere_output_precision, RADIANS * sp->lng,
					sphere_output_precision, RADIANS * sp->lat);
			break;

		case OUTPUT_DMS:
			rad_to_dms(sp->lng, &lngdeg, &lngmin, &lngsec);
			rad_to_dms(sp->lat, &latdeg, &latmin, &latsec);
			sprintf(buffer,
					"(%3ud %2um %.*gs , %c%2ud %2um %.*gs)",
					lngdeg, lngmin, sphere_output_precision, lngsec,
					(sp->lat < 0) ? ('-') : ('+'),
					latdeg, latmin, sphere_output_precision, latsec);
			break;

		case OUTPUT_HMS:
			rad_to_dms(sp->lng / 15, &lngdeg, &lngmin, &lngsec);
			rad_to_dms(sp->lat, &latdeg, &latmin, &latsec);
			sprintf(buffer,
					"(%3uh %2um %.*gs , %c%2ud %2um %.*gs)",
					lngdeg, lngmin, sphere_output_precision, lngsec,
					(sp->lat < 0) ? ('-') : ('+'),
					latdeg, latmin, sphere_output_precision, latsec);
			break;

		default:
			sprintf(buffer, "(%.*g , %.*g)",
					sphere_output_precision, sp->lng,
					sphere_output_precision, sp->lat);
			break;
	}

	PG_RETURN_CSTRING(buffer);
}

static void
spherepoint_out_deg(StringInfo si, const SPoint *sp)
{
	pgs_strinfo_put_chr(si, '(');
	pgs_strinfo_put_d64(si, RADIANS * sp->lng);
	pgs_strinfo_put_str(si, "d, ");
	pgs_strinfo_put_d64(si, RADIANS * sp->lat);
	pgs_strinfo_put_str(si, "d)");
}

static void
spherepoint_out_rad(StringInfo si, const SPoint *sp)
{
	pgs_strinfo_put_chr(si, '(');
	pgs_strinfo_put_d64(si, sp->lng);
	pgs_strinfo_put_str(si, " , ");
	pgs_strinfo_put_d64(si, sp->lat);
	pgs_strinfo_put_str(si, ")");
}

static void
spherepoint_out_dms(StringInfo si, const SPoint *sp)
{
	pgs_strinfo_put_chr(si, '(');
	pgs_strinfo_put_lng_dms(si, sp->lng);
	pgs_strinfo_put_str(si, " , ");
	pgs_strinfo_put_lat_dms(si, sp->lat);
	pgs_strinfo_put_chr(si, ')');
}

static void
spherepoint_out_hms(StringInfo si, const SPoint *sp)
{
	pgs_strinfo_put_chr(si, '(');
	pgs_strinfo_put_lng_hms(si, sp->lng);
	pgs_strinfo_put_str(si, " , ");
	pgs_strinfo_put_lat_dms(si, sp->lat);
	pgs_strinfo_put_chr(si, ')');
}

static inline void
spherepoint_out_buffer(StringInfo si, const SPoint *sp)
{
	switch (sphere_output)
	{
		case OUTPUT_DEG:
			spherepoint_out_deg(si, sp);
			break;
		case OUTPUT_DMS:
			spherepoint_out_dms(si, sp);
			break;
		case OUTPUT_HMS:
			spherepoint_out_hms(si, sp);
			break;
		default:
			spherepoint_out_rad(si, sp);
			break;
	}
}

Datum
spherepoint_out(PG_FUNCTION_ARGS)
{
	StringInfoData	si;
	SPoint		   *sp;

	if (sphere_output_precision != INT_MAX)
		return spherepoint_out_compat(fcinfo);

	sp = (SPoint *) PG_GETARG_POINTER(0);
	if (!sp)
		PG_RETURN_NULL();

	initStringInfo(&si);
	spherepoint_out_buffer(&si, sp);

	PG_RETURN_CSTRING(si.data);
}

static Datum
spherecircle_out_compat(PG_FUNCTION_ARGS)
{
	SCIRCLE    *c = (SCIRCLE *) PG_GETARG_POINTER(0);
	char	   *buffer = (char *) palloc(255);
	char	   *pointstr = DatumGetPointer(
								DirectFunctionCall1(spherepoint_out,
													PointerGetDatum(&c->center)));

	unsigned int rdeg,
				rmin;
	double		rsec;


	rdeg = rmin = 0;
	rsec = 0.0;

	switch (sphere_output)
	{

		case OUTPUT_DEG:
			sprintf(buffer, "<%s , %.*gd>",
					pointstr, sphere_output_precision,
					RADIANS * c->radius);
			break;

		case OUTPUT_HMS:
		case OUTPUT_DMS:
			rad_to_dms(c->radius, &rdeg, &rmin, &rsec);
			sprintf(buffer,
					"<%s , %2ud %2um %.*gs>",
					pointstr,
					rdeg, rmin, sphere_output_precision, rsec);
			break;

		default:
			sprintf(buffer,	"<%s , %.*g>",
					pointstr, sphere_output_precision, c->radius);
			break;
	}

	pfree(pointstr);

	PG_RETURN_CSTRING(buffer);
}

static void
spherecircle_out_deg(StringInfo si, const SCIRCLE *sc)
{
	pgs_strinfo_put_chr(si, '<');
	spherepoint_out_deg(si, &sc->center);
	pgs_strinfo_put_str(si, " , ");
	pgs_strinfo_put_d64(si, RADIANS * sc->radius);
	pgs_strinfo_put_chr(si, '>');
}

static void
spherecircle_out_rad(StringInfo si, const SCIRCLE *sc)
{
	pgs_strinfo_put_chr(si, '<');
	spherepoint_out_rad(si, &sc->center);
	pgs_strinfo_put_str(si, " , ");
	pgs_strinfo_put_d64(si, sc->radius);
	pgs_strinfo_put_chr(si, '>');
}

static void
spherecircle_out_dms(StringInfo si, const SCIRCLE *sc)
{
	pgs_strinfo_put_chr(si, '<');
	spherepoint_out_dms(si, &sc->center);
	pgs_strinfo_put_str(si, " , ");
	pgs_strinfo_put_radius_dms(si, sc->radius);
	pgs_strinfo_put_chr(si, '>');
}

static void
spherecircle_out_hms(StringInfo si, const SCIRCLE *sc)
{
	pgs_strinfo_put_chr(si, '<');
	spherepoint_out_hms(si, &sc->center);
	pgs_strinfo_put_str(si, " , ");
	pgs_strinfo_put_radius_dms(si, sc->radius);
	pgs_strinfo_put_chr(si, '>');
}

Datum
spherecircle_out(PG_FUNCTION_ARGS)
{
	StringInfoData	si;
	SCIRCLE		   *sc;

	if (sphere_output_precision != INT_MAX)
		return spherecircle_out_compat(fcinfo);

	sc = (SCIRCLE *) PG_GETARG_POINTER(0);
	if (!sc)
		PG_RETURN_NULL();

	initStringInfo(&si);

	switch (sphere_output)
	{
		case OUTPUT_DEG:
			spherecircle_out_deg(&si, sc);
			break;
		case OUTPUT_DMS:
			spherecircle_out_dms(&si, sc);
			break;
		case OUTPUT_HMS:
			spherecircle_out_hms(&si, sc);
			break;
		default:
			spherecircle_out_rad(&si, sc);
			break;
	}

	PG_RETURN_CSTRING(si.data);
}

static Datum
sphereellipse_out_compat(PG_FUNCTION_ARGS)
{
	SELLIPSE   *e = (SELLIPSE *) PG_GETARG_POINTER(0);
	char	   *buffer = (char *) palloc(255);
	char	   *pointstr;
	unsigned int rdeg[3],
				rmin[3];
	double		rsec[3];
	SPoint		sp;


	sp.lng = e->psi;
	sp.lat = -e->theta;
	pointstr = DatumGetPointer(DirectFunctionCall1(spherepoint_out,
												   PointerGetDatum(&sp)));

	switch (sphere_output)
	{

		case OUTPUT_DEG:
			sprintf(buffer,
					"<{ %.*gd , %.*gd }, %s , %.*gd>",
					sphere_output_precision, RADIANS * (e->rad[0]),
					sphere_output_precision, RADIANS * (e->rad[1]),
					pointstr,
					sphere_output_precision, RADIANS * (e->phi));
			break;

		case OUTPUT_HMS:
		case OUTPUT_DMS:
			rad_to_dms(e->rad[0], &rdeg[0], &rmin[0], &rsec[0]);
			rad_to_dms(e->rad[1], &rdeg[1], &rmin[1], &rsec[1]);
			rad_to_dms(e->phi, &rdeg[2], &rmin[2], &rsec[2]);
			sprintf(buffer,
					"<{ %2ud %2um %.*gs , %2ud %2um %.*gs }, %s , %2ud %2um %.*gs>",
					rdeg[0], rmin[0], sphere_output_precision, rsec[0],
					rdeg[1], rmin[1], sphere_output_precision, rsec[1],
					pointstr,
					rdeg[2], rmin[2], sphere_output_precision, rsec[2]);
			break;

		default:
			sprintf(
				buffer,
				"<{ %.*g , %.*g }, %s , %.*g>",
				sphere_output_precision, e->rad[0],
				sphere_output_precision, e->rad[1],
				pointstr,
				sphere_output_precision, e->phi);
			break;
	}

	pfree(pointstr);
	PG_RETURN_CSTRING(buffer);
}

static void
sphereellipse_out_deg(StringInfo si, SELLIPSE *e)
{
	const SPoint sp = { e->psi, e->theta };

	pgs_strinfo_put_str(si, "<{ ");
	pgs_strinfo_put_d64(si, RADIANS * e->rad[0]);
	pgs_strinfo_put_str(si, "d , ");
	pgs_strinfo_put_d64(si, RADIANS * e->rad[1]);
	pgs_strinfo_put_str(si, "d },");
	spherepoint_out_buffer(si, &sp);
	pgs_strinfo_put_str(si, " , ");
	pgs_strinfo_put_d64(si, RADIANS * e->phi);
	pgs_strinfo_put_str(si, "d>");
}

static void
sphereellipse_out_rad(StringInfo si, SELLIPSE *e)
{
	const SPoint sp = { e->psi, e->theta };

	pgs_strinfo_put_str(si, "<{ ");
	pgs_strinfo_put_d64(si, e->rad[0]);
	pgs_strinfo_put_str(si, " , ");
	pgs_strinfo_put_d64(si, e->rad[1]);
	pgs_strinfo_put_str(si, " },");
	spherepoint_out_buffer(si, &sp);
	pgs_strinfo_put_str(si, " , ");
	pgs_strinfo_put_d64(si, e->phi);
	pgs_strinfo_put_str(si, ">");
}

static void
sphereellipse_out_dms(StringInfo si, SELLIPSE *e)
{
	const SPoint sp = { e->psi, e->theta };

	pgs_strinfo_put_str(si, "<{ ");
	pgs_strinfo_put_lng_dms(si, e->rad[0]);
	pgs_strinfo_put_str(si, " , ");
	pgs_strinfo_put_lng_dms(si, e->rad[1]);
	pgs_strinfo_put_str(si, " },");
	spherepoint_out_buffer(si, &sp);
	pgs_strinfo_put_str(si, " , ");
	pgs_strinfo_put_lng_dms(si, e->phi);
	pgs_strinfo_put_str(si, ">");
}

Datum
sphereellipse_out(PG_FUNCTION_ARGS)
{
	StringInfoData	si;
	SELLIPSE	   *e;

	if (sphere_output_precision != INT_MAX)
		return sphereellipse_out_compat(fcinfo);

	e = (SELLIPSE *) PG_GETARG_POINTER(0);
	if (!e)
		PG_RETURN_NULL();

	initStringInfo(&si);

	switch (sphere_output)
	{
		case OUTPUT_DEG:
			sphereellipse_out_deg(&si, e);
			break;
		case OUTPUT_HMS:
		case OUTPUT_DMS:
			sphereellipse_out_dms(&si, e);
			break;
		default:
			sphereellipse_out_rad(&si, e);
			break;
	}

	PG_RETURN_CSTRING(si.data);
}

static Datum
sphereline_out_compat(PG_FUNCTION_ARGS)
{
	SLine		   *sl = (SLine *) PG_GETARG_POINTER(0);
	char		   *out = (char *) palloc(255);
	char		   *tstr = NULL;
	SEuler			se;

	unsigned int	rdeg,
					rmin;
	double			rsec;

	rdeg = rmin = 0;
	rsec = 0.0;

	seuler_set_zxz(&se);
	se.phi = sl->phi;
	se.theta = sl->theta;
	se.psi = sl->psi;
	tstr = DatumGetPointer(DirectFunctionCall1(spheretrans_out,
											   PointerGetDatum(&se)));

	switch (sphere_output)
	{
		case OUTPUT_DEG:
			sprintf(out,
					"( %s ), %.*gd",
					tstr, sphere_output_precision, RADIANS * sl->length
				);
			break;

		case OUTPUT_HMS:
		case OUTPUT_DMS:
			rad_to_dms(sl->length, &rdeg, &rmin, &rsec);
			sprintf(out,
					"( %s ), %2ud %2um %.*gs",
					tstr, rdeg, rmin, sphere_output_precision, rsec);
			break;

		default:
			sprintf(out,
					"( %s ), %.*g",
					tstr, sphere_output_precision, sl->length
				);
			break;
	}

	PG_RETURN_CSTRING(out);
}

Datum
sphereline_out(PG_FUNCTION_ARGS)
{
	StringInfoData	si;
	SLine		   *sl;
	SEuler			se;

	if (sphere_output_precision != INT_MAX)
		return sphereline_out_compat(fcinfo);

	sl = (SLine *) PG_GETARG_POINTER(0);
	if (!sl)
		PG_RETURN_NULL();

	seuler_set_zxz(&se);
	se.phi = sl->phi;
	se.theta = sl->theta;
	se.psi = sl->psi;

	initStringInfo(&si);

	pgs_strinfo_put_str(&si, "( ");
	spheretrans_out_buffer(&si, &se);
	pgs_strinfo_put_str(&si, " ), ");

	switch (sphere_output)
	{
		case OUTPUT_DEG:
			pgs_strinfo_put_d64(&si, RADIANS * sl->length);
			break;
		case OUTPUT_HMS:
		case OUTPUT_DMS:
			pgs_strinfo_put_lng_dms(&si, sl->length);
			break;
		default:
			pgs_strinfo_put_d64(&si, sl->length);
			break;
	}

	PG_RETURN_CSTRING(si.data);
}

static Datum
spheretrans_out_compat(PG_FUNCTION_ARGS)
{
	SEuler	   *se = (SEuler *) PG_GETARG_POINTER(0);
	char	   *buffer = (char *) palloc(255);
	char		buf[100];
	char		etype[4];
	SPoint		val[3];
	unsigned char i,
				t = 0;
	unsigned int rdeg,
				rmin;
	double		rsec;

	val[0].lat = val[1].lat = val[2].lat = 0.0;
	val[0].lng = se->phi;
	val[1].lng = se->theta;
	val[2].lng = se->psi;

	spoint_check(&val[0]);
	spoint_check(&val[1]);
	spoint_check(&val[2]);

	buffer[0] = '\0';
	for (i = 0; i < 3; i++)
	{

		rdeg = rmin = 0;
		rsec = 0.0;
		switch (sphere_output)
		{

			case OUTPUT_DEG:
				sprintf(&buf[0],
						"%.*gd",
						sphere_output_precision, RADIANS * val[i].lng);
				break;

			case OUTPUT_HMS:
			case OUTPUT_DMS:
				rad_to_dms(val[i].lng, &rdeg, &rmin, &rsec);
				sprintf(&buf[0],
						"%2ud %2um %.*gs",
						rdeg, rmin, sphere_output_precision, rsec);
				break;

			default:
				sprintf(&buf[0], "%.*g", sphere_output_precision, val[i].lng);
				break;
		}
		strcat(&buf[0], ", ");
		strcat(buffer, &buf[0]);
	}
	for (i = 0; i < 3; i++)
	{
		switch (i)
		{
			case 0:
				t = se->phi_a;
				break;
			case 1:
				t = se->theta_a;
				break;
			case 2:
				t = se->psi_a;
				break;
		}
		switch (t)
		{
			case EULER_AXIS_X:
				etype[i] = 'X';
				break;
			case EULER_AXIS_Y:
				etype[i] = 'Y';
				break;
			case EULER_AXIS_Z:
				etype[i] = 'Z';
				break;
		}
	}
	etype[3] = '\0';
	strcat(buffer, etype);

	PG_RETURN_CSTRING(buffer);
}

static void
spheretrans_out_deg(StringInfo si, SPoint sp[3])
{
	int			i;

	for (i = 0; i < 3; ++i)
	{
		pgs_strinfo_put_d64(si, RADIANS * sp[i].lng);
		pgs_strinfo_put_str(si, ", ");
	}
}

static void
spheretrans_out_rad(StringInfo si, SPoint sp[3])
{
	int			i;

	for (i = 0; i < 3; ++i)
	{
		pgs_strinfo_put_d64(si, sp[i].lng);
		pgs_strinfo_put_str(si, ", ");
	}
}

static void
spheretrans_out_dms(StringInfo si, SPoint sp[3])
{
	int				i;

	for (i = 0; i < 3; ++i)
	{
		pgs_strinfo_put_lng_dms(si, sp[i].lng);
		pgs_strinfo_put_str(si, ", ");
	}
}

static void
spheretrans_out_buffer(StringInfo si, const SEuler *se)
{
	SPoint			val[3];
	unsigned char	t[3];
	int				i;

	val[0].lat = val[1].lat = val[2].lat = 0.0;
	val[0].lng = se->phi;
	val[1].lng = se->theta;
	val[2].lng = se->psi;

	t[0] = se->phi_a;
	t[1] = se->theta_a;
	t[2] = se->psi_a;

	spoint_check(&val[0]);
	spoint_check(&val[1]);
	spoint_check(&val[2]);

	switch (sphere_output)
	{
		case OUTPUT_DEG:
			spheretrans_out_deg(si, val);
			break;
		case OUTPUT_HMS:
		case OUTPUT_DMS:
			spheretrans_out_dms(si, val);
			break;
		default:
			spheretrans_out_rad(si, val);
			break;
	}

	for (i = 0; i < 3; i++)
	{
		switch (t[i])
		{
			case EULER_AXIS_X:
				pgs_strinfo_put_chr(si, 'X');
				break;
			case EULER_AXIS_Y:
				pgs_strinfo_put_chr(si, 'Y');
				break;
			case EULER_AXIS_Z:
				pgs_strinfo_put_chr(si, 'Z');
				break;
			default:
				Assert(false);
		}
	}
}

Datum
spheretrans_out(PG_FUNCTION_ARGS)
{
	StringInfoData	si;
	SEuler		   *se;

	if (sphere_output_precision != INT_MAX)
		return spheretrans_out_compat(fcinfo);

	se = (SEuler *) PG_GETARG_POINTER(0);
	if (!se)
		PG_RETURN_NULL();

	initStringInfo(&si);

	spheretrans_out_buffer(&si, se);

	PG_RETURN_CSTRING(si.data);
}

static Datum
spherepath_out_compat(PG_FUNCTION_ARGS)
{
	SPATH	   *path = PG_GETARG_SPATH(0);
	int32		i;
	char	   *out = (char *) palloc(128 * path->npts);
	char	   *tmp;

	strcpy(out, "{");
	for (i = 0; i < path->npts; i++)
	{
		if (i > 0)
		{
			strcat(out, ",");
		}
		tmp = DatumGetPointer(DirectFunctionCall1(spherepoint_out,
												  PointerGetDatum(&path->p[i])));
		strcat(out, tmp);
		pfree(tmp);
	}
	strcat(out, "}");
	PG_RETURN_CSTRING(out);
}

Datum
spherepath_out(PG_FUNCTION_ARGS)
{
	StringInfoData	si;
	SPATH		   *path;
	int32			i;

	if (sphere_output_precision != INT_MAX)
		return spherepath_out_compat(fcinfo);

	path = PG_GETARG_SPATH(0);
	if (!path)
		PG_RETURN_NULL();

	initStringInfo(&si);

	pgs_strinfo_put_chr(&si, '{');

	for (i = 0; i < path->npts; ++i)
	{
		if (i > 0)
			pgs_strinfo_put_chr(&si, ',');
		spherepoint_out_buffer(&si, &path->p[i]);
	}

	pgs_strinfo_put_chr(&si, '}');

	PG_RETURN_CSTRING(si.data);
}

static Datum
spherepoly_out_compat(PG_FUNCTION_ARGS)
{
	SPOLY	   *poly = PG_GETARG_SPOLY(0);
	int32		i;
	char	   *out = (char *) palloc(128 * poly->npts);
	char	   *tmp;

	strcpy(out, "{");
	for (i = 0; i < poly->npts; i++)
	{
		if (i > 0)
		{
			strcat(out, ",");
		}
		tmp = DatumGetPointer(DirectFunctionCall1(spherepoint_out,
												  PointerGetDatum(&poly->p[i])));
		strcat(out, tmp);
		pfree(tmp);
	}
	strcat(out, "}");
	PG_RETURN_CSTRING(out);
}

Datum
spherepoly_out(PG_FUNCTION_ARGS)
{
	StringInfoData	si;
	SPOLY		   *poly;
	int32			i;

	if (sphere_output_precision != INT_MAX)
		return spherepoly_out_compat(fcinfo);

	poly = PG_GETARG_SPOLY(0);
	if (!poly)
		PG_RETURN_NULL();

	initStringInfo(&si);

	pgs_strinfo_put_chr(&si, '{');

	for (i = 0; i < poly->npts; ++i)
	{
		if (i > 0)
			pgs_strinfo_put_chr(&si, ',');
		spherepoint_out_buffer(&si, &poly->p[i]);
	}

	pgs_strinfo_put_chr(&si, '}');

	PG_RETURN_CSTRING(si.data);
}

static Datum
spherebox_out_compat(PG_FUNCTION_ARGS)
{
	SBOX	   *box = (SBOX *) PG_GETARG_POINTER(0);
	char	   *buffer = (char *) palloc(255);
	char	   *str1 = DatumGetPointer(
							DirectFunctionCall1(spherepoint_out,
												PointerGetDatum(&box->sw)));
	char	   *str2 = DatumGetPointer(
							DirectFunctionCall1(spherepoint_out,
												PointerGetDatum(&box->ne)));

	sprintf(buffer, "(%s, %s)", str1, str2);
	pfree(str1);
	pfree(str2);
	PG_RETURN_CSTRING(buffer);
}

Datum
spherebox_out(PG_FUNCTION_ARGS)
{
	StringInfoData	si;
	SBOX		   *box;

	if (sphere_output_precision != INT_MAX)
		return spherebox_out_compat(fcinfo);

	box = (SBOX *) PG_GETARG_POINTER(0);
	if (!box)
		PG_RETURN_NULL();

	initStringInfo(&si);

	pgs_strinfo_put_chr(&si, '(');
	spherepoint_out_buffer(&si, &box->sw);
	pgs_strinfo_put_str(&si, ", ");
	spherepoint_out_buffer(&si, &box->ne);
	pgs_strinfo_put_chr(&si, ')');

	PG_RETURN_CSTRING(si.data);
}

Datum
pg_sphere_version(PG_FUNCTION_ARGS)
{
	const char	*s = PGSPHERE_STRINGIFY(PGSPHERE_VERSION);
	char		*p = (char *)palloc(strlen(s) + 1);
	strcpy(p, s);
	PG_RETURN_CSTRING(p);
}
