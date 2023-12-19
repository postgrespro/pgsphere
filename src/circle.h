#ifndef __PGS_CIRCLE_H__
#define __PGS_CIRCLE_H__

#include "euler.h"

/* Spherical circle declarations */

/*
 * Spherical circle data structure: center and radius.
 */
typedef struct
{
	SPoint		center;			/* the center of circle */
	float8		radius;			/* the circle radius in radians */
} SCIRCLE;

/*
 * Checks whether two circles are equal.
 */
extern bool scircle_eq(const SCIRCLE *c1, const SCIRCLE *c2);

/*
 * Checks whether a circle contains a point.
 */
extern bool spoint_in_circle(const SPoint *p, const SCIRCLE *c);

/*
 * Transforms a circle using an Euler transformation.
 */
extern void euler_scircle_trans(SCIRCLE *out, const SCIRCLE *in, const SEuler *se);

/*
 * Takes the input and stores it as a spherical circle.
 */
extern Datum spherecircle_in(PG_FUNCTION_ARGS);

/*
 * Checks whether two circles are equal.
 */
extern Datum spherecircle_equal(PG_FUNCTION_ARGS);

/*
 * Checks whether two circles are not equal.
 */
extern Datum spherecircle_equal_neg(PG_FUNCTION_ARGS);

/*
 * Calculate the distance of two circles. If they overlap, this function
 * returns 0.0.
 */
extern Datum spherecircle_distance(PG_FUNCTION_ARGS);

/*
 * Calculate the distance of a circle and a point. If a circle contains a point,
 * this function returns 0.0.
 */
extern Datum spherecircle_point_distance(PG_FUNCTION_ARGS);

/*
 * Calculate the distance of a point and a circle. If a circle contains a point,
 * this function returns 0.0.
 */
extern Datum spherecircle_point_distance_com(PG_FUNCTION_ARGS);

/*
 * Checks whether a circle contains a point.
 */
extern Datum spherepoint_in_circle(PG_FUNCTION_ARGS);

/*
 * Checks whether a circle doesn't contain a point.
 */
extern Datum spherepoint_in_circle_neg(PG_FUNCTION_ARGS);

/*
 * Checks whether a circle contains a point.
 */
extern Datum spherepoint_in_circle_com(PG_FUNCTION_ARGS);

/*
 * Checks whether a circle doesn't contain a point.
 */
extern Datum spherepoint_in_circle_com_neg(PG_FUNCTION_ARGS);

/*
 * Checks whether a circle is contained by other circle.
 */
extern Datum spherecircle_in_circle(PG_FUNCTION_ARGS);

/*
 * Checks whether a circle is not contained by other circle.
 */
extern Datum spherecircle_in_circle_neg(PG_FUNCTION_ARGS);

/*
 * Checks whether a circle contains other circle.
 */
extern Datum spherecircle_in_circle_com(PG_FUNCTION_ARGS);

/*
 * Checks whether circle does not contain other circle.
 */
extern Datum spherecircle_in_circle_com_neg(PG_FUNCTION_ARGS);

/*
 * Checks whether two circles overlap.
 */
extern Datum spherecircle_overlap(PG_FUNCTION_ARGS);

/*
 * Checks whether two circles overlap.
 */
extern Datum spherecircle_overlap_neg(PG_FUNCTION_ARGS);

/*
 * Returns the center of a circle.
 */
extern Datum spherecircle_center(PG_FUNCTION_ARGS);

/*
 * Returns the radius of a circle.
 */
extern Datum spherecircle_radius(PG_FUNCTION_ARGS);

/*
 * Converts a point to a circle.
 */
extern Datum spherepoint_to_circle(PG_FUNCTION_ARGS);

/*
 * Creates a circle from center and radius.
 */
extern Datum spherecircle_by_center(PG_FUNCTION_ARGS);

/*
 * Creates a circle from center and radius(in degrees).
 */
extern Datum spherecircle_by_center_deg(PG_FUNCTION_ARGS);

/*
 * Calculates the area of a circle in square radians.
 */
extern Datum spherecircle_area(PG_FUNCTION_ARGS);

/*
 * Calculates the circumference of a circle in radians.
 */
extern Datum spherecircle_circ(PG_FUNCTION_ARGS);

/*
 * Transforms a circle using an Euler transformation.
 */
extern Datum spheretrans_circle(PG_FUNCTION_ARGS);

/*
 * Inverse transformation of a circle using an Euler transformation.
 */
extern Datum spheretrans_circle_inverse(PG_FUNCTION_ARGS);

#endif
