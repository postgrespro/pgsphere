/* Handling of (conventional) proper motions.

This code is largely based on a FORTRAN function written by Lennart Lindegren
(Lund Obs) in 1995 that implements the procedure described in The Hipparcos
and Tycho Catalogues (ESA SP-1200), Volume 1, Section 1.5.5, 'Epoch
Transformation: Rigorous Treatment'; cf.
<https://www.cosmos.esa.int/documents/532822/552851/vol1_all.pdf/99adf6e3-6893-4824-8fc2-8d3c9cbba2b5>.
*/

#include <math.h>
#include <pgs_util.h>

#include "point.h"
#include "epochprop.h"
#include "vector3d.h"

PG_FUNCTION_INFO_V1(epoch_prop);

/* Astronomical unit in kilometers */
#define AU 1.495978701e8

/* Julian year in seconds */
#define J_YEAR (365.25*86400)

/* A_nu as per ESA/SP-1200 */
#define A_NU (AU/(J_YEAR))

/* Following SOFA, we use 1e-7 arcsec as minimal parallax 
	("celestial sphere"); parallax=0 exactly means "infinite distance", which
	leads to all sorts for problems; our parallaxes come in in mas, so: */
#define PX_MIN 1e-7*1000

/* propagate an object at a phase vector over a time difference of delta_t,
stuffing an updated phase vector in result.

This does not propagate errors.
*/
static void propagate_phasevec(
	const phasevec *pv,
	const double delta_t,
	phasevec *result) {

	double distance_factor, mu0abs, zeta0, parallax;

	Vector3D p0, r0, q0;
	Vector3D mu0, pprime, qprime, mu, muprime, u, uprime;

	/* for very small or null parallaxes, our algorithm breaks; avoid that
		and, if we did emergency measures, do not talk about parallax and
		radial velocity in the output */
	if (pv->parallax_valid) {
		parallax = pv->parallax;
	} else {
		parallax = PX_MIN;
	}
	result->parallax_valid = pv->parallax_valid;

	/* compute the normal triad as Vector3D-s, eq. (1.2.15)*/
	spoint_vector3d(&r0, &(pv->pos));

	p0.x = -sin(pv->pos.lng);
	p0.y = cos(pv->pos.lng); 
	p0.z = 0;

	q0.x = -sin(pv->pos.lat) * cos(pv->pos.lng);
	q0.y = -sin(pv->pos.lat) * sin(pv->pos.lng); 
	q0.z = cos(pv->pos.lat);

	/* the original proper motion vector */
	mu0.x = mu0.y = mu0.z = 0;
	vector3d_addwithscalar(&mu0, pv->pm[0], &p0);
	vector3d_addwithscalar(&mu0, pv->pm[1], &q0);
	mu0abs = vector3d_length(&mu0);

	/* radial velocity in mas/yr ("change of parallax per year").  eq. (1.5.24)
		We're transforming this to rad/yr so the units work out below */
	zeta0 = (pv->rv * parallax / A_NU) / 3.6e6 / RADIANS;
	/* distance factor eq. (1.5.25) */
	distance_factor = 1/sqrt(1
		+ 2 * zeta0 * delta_t
		+ (mu0abs * mu0abs + zeta0 * zeta0) * delta_t * delta_t);

	/* the propagated proper motion vector, eq. (1.5.28) */
	muprime.x = muprime.y = muprime.z = 0;
	vector3d_addwithscalar(&muprime, (1 + zeta0 * delta_t), &mu0);
	vector3d_addwithscalar(&muprime, -mu0abs * mu0abs * delta_t, &r0);
	mu.x = mu.y = mu.z = 0;
	vector3d_addwithscalar(&mu, pow(distance_factor, 3), &muprime);

	/* parallax, eq. (1.5.27) */
	result->parallax = distance_factor*parallax;
	/* zeta/rv, eq. (1.5.29); go back from rad to mas, too */
	result->rv = (zeta0 + (mu0abs * mu0abs + zeta0 * zeta0) * delta_t)
		* distance_factor * distance_factor
		* 3.6e6 * RADIANS
		* A_NU / result->parallax;

	/* propagated position, eq. (1.5.26) */
	uprime.x = uprime.y = uprime.z = 0;
	vector3d_addwithscalar(&uprime, (1 + zeta0 * delta_t), &r0);
	vector3d_addwithscalar(&uprime, delta_t, &mu0);
	u.x = u.y = u.z = 0;
	vector3d_addwithscalar(&u, distance_factor, &uprime);
	vector3d_spoint(&(result->pos), &u);

	/* compute a new triad for the propagated position, eq (1.5.15) */
	pprime.x = -sin(result->pos.lng);
	pprime.y = cos(result->pos.lng);
	pprime.z = 0;
	qprime.x = -sin(result->pos.lat) * cos(result->pos.lng);
	qprime.y = -sin(result->pos.lat) * sin(result->pos.lng);
	qprime.z = cos(result->pos.lat);

	/* use it to compute the proper motions, eq. (1.5.32) */
	result->pm[0] = vector3d_scalar(&pprime, &mu);
	result->pm[1] = vector3d_scalar(&qprime, &mu);
}


/* 
	Propagate a position with proper motions and optionally parallax
	and radial velocity.

	Arguments: pos0 (spoint), pm_long, pm_lat (in rad/yr)
	par (parallax, mas), rv (in km/s), delta_t (in years) 

	This returns a 6-array of lat, long (in rad), parallax (in mas)
	pmlat, pmlong (in rad/yr), rv (in km/s).
*/
Datum 
epoch_prop(PG_FUNCTION_ARGS) {
	double delta_t;
	phasevec input, output;
	ArrayType *result;
	Datum retvals[6];

	if (PG_ARGISNULL(0)) {
		ereport(ERROR, 
			(errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED),
				errmsg("NULL position not supported in epoch propagation"))); }
	memcpy(&(input.pos), (void*)PG_GETARG_POINTER(0), sizeof(SPoint));
	if (PG_ARGISNULL(1)) {
		input.parallax = 0;
	} else {
		input.parallax = PG_GETARG_FLOAT8(1);
	}
	input.parallax_valid = fabs(input.parallax) > PX_MIN;
	
	if (PG_ARGISNULL(2)) {
		input.pm[0] = 0;
	} else {
		input.pm[0] = PG_GETARG_FLOAT8(2);
	}

	if (PG_ARGISNULL(3)) {
		input.pm[1] = 0;
	} else {
		input.pm[1] = PG_GETARG_FLOAT8(3);
	}

	if (PG_ARGISNULL(4)) {
		input.rv = 0;
	} else {
		input.rv = PG_GETARG_FLOAT8(4);
	}

	delta_t = PG_GETARG_FLOAT8(5);

	propagate_phasevec(&input, delta_t, &output);

	/* change to internal units: rad, rad/yr, mas, and km/s */
	retvals[0] = Float8GetDatum(output.pos.lng);
	retvals[1] = Float8GetDatum(output.pos.lat);
	retvals[2] = Float8GetDatum(output.parallax);
	retvals[3] = Float8GetDatum(output.pm[0]);
	retvals[4] = Float8GetDatum(output.pm[1]);
	retvals[5] = Float8GetDatum(output.rv);

	{
		bool isnull[6] = {0, 0, 0, 0, 0, 0};
		int lower_bounds[1] = {1};
		int dims[1] = {6};
#ifdef USE_FLOAT8_BYVAL
		bool embyval = true;
#else
		bool embyval = false;
#endif

		if (! output.parallax_valid) {
			/* invalidate parallax and rv */
			isnull[2] = 1;
			isnull[5] = 1;
		}

		result = construct_md_array(retvals, isnull, 1, dims, lower_bounds,
			FLOAT8OID, sizeof(float8), embyval, 'd');
	}
	PG_RETURN_ARRAYTYPE_P(result);
}
