#ifndef __PGS_CHEALPIX_H__
#define __PGS_CHEALPIX_H__

#include <healpix_bare/healpix_bare.h>
#include <c.h>                                        /* PostgreSQL type definitions */
/*
 * Actually, chealpix changed its API: thus, this file must be included first,
 * directly or indirectly.
 */

#define MOC_FORMAT_64 INT64_FORMAT
#define MOC_FORMAT_64U UINT64_FORMAT

typedef int64 hpint64;

#ifdef UINT32_MAX
#define MOC_FORMAT_64X "lx"
#else
#define MOC_FORMAT_64X "llx"
#endif

#endif
