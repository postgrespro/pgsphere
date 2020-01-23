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
#include "utils/geo_decls.h"
#include "utils/array.h"
#include "utils/elog.h"
#include "utils/builtins.h"
#include "catalog/pg_type.h"
#include "access/skey.h"
#include "access/gist.h"
#include "access/itup.h"

#include "pgs_util.h"

void	sphere_yyparse(void);

#endif
