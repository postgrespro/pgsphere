#ifndef __PGS_PGSPHERE_H__
#define __PGS_PGSPHERE_H__

/* Base declarations and includes */


/*
 * pgSphere is an extra module for PostgreSQL which adds spherical data types. It provides:
 *
 * 		- input and output of data
 * 		- containing, overlapping, and other operators
 * 		- various input and converting functions and operators
 * 		- circumference and area of an object
 * 		- spherical transformation
 * 		- indexing of spherical data types
 * 		- several input and output formats
 *
 * Hence, you can do a fast search and analysis for objects with spherical
 * attributes as used in geographical, astronomical, or other applications
 * using PostgreSQL. For instance, you can manage data of geographical
 * objects around the world and astronomical data like star and other
 * catalogs conveniently using an SQL interface.
 *
 * The aim of pgSphere is to
 * provide uniform access to spherical data. Because PostgreSQL itself
 * supports a lot of software interfaces, you can now use the same database
 * with different utilities and applications.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <float.h>

#include "postgres.h"
#include "fmgr.h"
#include "utils/array.h"
#include "utils/elog.h"
#include "utils/builtins.h"
#include "catalog/pg_type.h"
#include "access/skey.h"
#include "access/gist.h"
#include "access/itup.h"

#include "pgs_util.h"

/* On some 32 bit platforms, there is a gcc bug that makes floating point
 * calculations and comparisons unstable (see the link below). The problem
 * originates in FPU 80 bits registers where double values are not truncated
 * to 64 bit values. When gcc compiles some code with enabled optimizations,
 * the intermediate results may be kept in the FPU registers without truncation
 * to 64 bit values. Extra bits may produce unstable results when comparing
 * the numbers.
 *
 * The generic solution is to save the intermediate results in the memory where
 * the values are truncated to 64 bit values. It affects the performance but
 * makes the tests stable on all platforms.
 *
 * PGSPHERE_FLOAT_STORE macro enables storing of intermediate results for FPxx
 * operations in the memory. It is enabled by default for 32 bit platforms.
 * It can be explicitly enabled or disabled in CFLAGS. To enable it for all
 * code the gcc option -ffloat-store may be used as well.
 *
 * Link to gcc bug: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=323
 */
#if !defined(PGSPHERE_FLOAT_STORE)
#if _WIN64 || (__GNUC__ && __x86_64__)
#define PGSPHERE_FLOAT_STORE 0
#elif _WIN32 || __GNUC__
#define PGSPHERE_FLOAT_STORE 1
#else
#define PGSPHERE_FLOAT_STORE 0
#endif
#endif // PGSPHERE_FLOAT_STORE

#define EPSILON					1.0E-09

#define FPzero(A)				(fabs(A) <= EPSILON)

#if PGSPHERE_FLOAT_STORE

static inline bool
FPeq(double A, double B)
{
	const volatile double AB = A - B;
	return A == B || fabs(AB) <= EPSILON;
}

static inline bool
FPne(double A, double B)
{
	const volatile double AB = A - B;
	return A != B && fabs(AB) > EPSILON;
}

static inline bool
FPlt(double A, double B)
{
	const volatile double AE = A + EPSILON;
	return AE < B;
}

static inline bool
FPle(double A, double B)
{
	const volatile double BE = B + EPSILON;
	return A <= BE;
}

static inline bool
FPgt(double A, double B)
{
	const volatile double BE = B + EPSILON;
	return A > BE;
}

static inline bool
FPge(double A, double B)
{
	const volatile double AE = A + EPSILON;
	return AE >= B;
}

#else

static inline bool
FPeq(double A, double B)
{
	return A == B || fabs(A - B) <= EPSILON;
}

static inline bool
FPne(double A, double B)
{
	return A != B && fabs(A - B) > EPSILON;
}

static inline bool
FPlt(double A, double B)
{
	return A + EPSILON < B;
}

static inline bool
FPle(double A, double B)
{
	return A <= B + EPSILON;
}

static inline bool
FPgt(double A, double B)
{
	return A > B + EPSILON;
}

static inline bool
FPge(double A, double B)
{
	return A + EPSILON >= B;
}

#endif // PGSPHERE_FLOAT_STORE

/*---------------------------------------------------------------------
 * Point - (x,y)
 *-------------------------------------------------------------------*/
typedef struct
{
	float8		x,
				y;
} Point;

void	sphere_yyparse(void);

#endif

