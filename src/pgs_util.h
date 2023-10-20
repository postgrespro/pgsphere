#ifndef __PGS_UTIL_H__
#define __PGS_UTIL_H__

#define PI			3.14159265358979323846		/* pi */
#define PIH			1.57079632679489661923		/* pi/2 */
#define PID			6.2831853071795864769		/* 2*pi */
#define RADIANS		57.295779513082320877		/* 180/pi */
#define PI_EPS		4.4408920985006261617e-16	/* 2 ** -51 */

#define Sqr(a)	 	( (a) * (a) )		/* square function as macro */

#ifdef EPSILON
#undef EPSILON
#endif
#define EPSILON		1.0E-09				/* precision of floating point values */

/* spherical circle constants */
#define SPHERE_SURFACE (4 * PI)
#define DEFAULT_SCIRCLE_SEL 1e-7

/* convert pg_sphere theta [pi/2 .. -pi/2] to healpix theta [0 .. pi] ([north .. south pole]) */
static inline double
conv_theta(double x)
{
	double y = PIH - x;
	if (fabs(x) < PI_EPS / 2)
		return PIH;
	if (fabs(y) < PI_EPS / 2)
		return 0;
	return y;
}

static inline double deg_to_rad(double in)
{
	return in * PI / 180;
}

/*
 * Area of circle on sphere
 */
static inline double
spherecircle_area_float(double radius)
{
	return PID * (1 - cos(radius));
}

#endif
