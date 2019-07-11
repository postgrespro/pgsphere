#ifndef __PGS_CHEALPIX_H__
#define __PGS_CHEALPIX_H__

#include <healpix_bare.h>
/*
 * Actually, chealpix changed its API: thus, this file must be included first,
 * directly or indirectly.
 */

#ifdef UINT32_MAX
typedef int64_t hpint64;
#define MOC_FORMAT_64 "%ld"
#define MOC_FORMAT_64U "%lu"
#define MOC_FORMAT_64X "lx"
#else
#define MOC_FORMAT_64 "%lld" 
#define MOC_FORMAT_64U "%llu"
#define MOC_FORMAT_64X "llx"
#endif

#endif
