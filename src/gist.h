#ifndef __PGS_GIST_H__
#define __PGS_GIST_H__

#include "key.h"

/*
 * GIST index declarations
 */

/* An alias for unsigned char */
typedef unsigned char uchar;

 /* PGS_KEY_REL Key relationships */
#define SCKEY_DISJ		0		/* two keys are disjunct */
#define SCKEY_OVERLAP	1		/* two keys are overlapping */
#define SCKEY_IN		2		/* first key contains second key */
#define SCKEY_SAME		3		/* keys are equal */

extern uchar spherekey_interleave(const int32 *k1, const int32 *k2);

/*
 * For given "query" of "pgstype" of PGS_DATA_TYPES type, puts key of cached
 * query into "key" pointer. Returns true when given query is equal to
 * current query.
 */
extern bool gq_cache_get_value(unsigned pgstype, const void *query, int32 **key);

/*
 * Copy current query, type and its key value to cache.
 */
extern void gq_cache_set_value(unsigned pgstype, const void *query, const int32 *key);

/*
 * Input function of key value. Just a dummy. But PostgreSQL need this
 * function to create a data type.
 */
extern Datum spherekey_in(PG_FUNCTION_ARGS);

/*
 * Output function of key value. Just a dummy. But PostgreSQL need this
 * function to create a data type.
 */
extern Datum spherekey_out(PG_FUNCTION_ARGS);

/*
 * GIST's decompress method. This function does nothing.
 */
extern Datum g_spherekey_decompress(PG_FUNCTION_ARGS);

/*
 * GIST's compress method for circle. Creates the key value from a spherical
 * circle.
 */
extern Datum g_scircle_compress(PG_FUNCTION_ARGS);

/*
 * GIST's compress method for point. Creates the key value from a spherical point.
 */
extern Datum g_spoint_compress(PG_FUNCTION_ARGS);

/*
 * GIST's compress method for line. Creates the key value from a spherical line.
 */
extern Datum g_sline_compress(PG_FUNCTION_ARGS);

/*
 * GIST's compress method for path. Creates the key value from a spherical path.
 */
extern Datum g_spath_compress(PG_FUNCTION_ARGS);

/*
 * GIST's compress method for polygon. Creates the key value from a spherical
 * polygon.
 */
extern Datum g_spoly_compress(PG_FUNCTION_ARGS);

/*
 * GIST's compress method for ellipse. Creates the key value from a spherical
 * ellipse.
 */
extern Datum g_sellipse_compress(PG_FUNCTION_ARGS);

/*
 * GIST's compress method for box. Creates the key value from a spherical box.
 */
extern Datum g_sbox_compress(PG_FUNCTION_ARGS);

/*
 * The GiST Union method for boxes. Returns the minimal bounding box that
 * encloses all the entries in entryvec.
 */
extern Datum g_spherekey_union(PG_FUNCTION_ARGS);

/*
 * GIST's equality method.
 */
extern Datum g_spherekey_same(PG_FUNCTION_ARGS);

/*
 * GIST's consistent method for a point.
 */
extern Datum g_spoint_consistent(PG_FUNCTION_ARGS);

/*
 * GIST's consistent method for a circle.
 */
extern Datum g_scircle_consistent(PG_FUNCTION_ARGS);

/*
 * GIST's consistent method for a line.
 */
extern Datum g_sline_consistent(PG_FUNCTION_ARGS);

/*
 * GIST's consistent method for a path.
 */
extern Datum g_spath_consistent(PG_FUNCTION_ARGS);

/*
 * GIST's consistent method for a polygon.
 */
extern Datum g_spoly_consistent(PG_FUNCTION_ARGS);

/*
 * GIST's consistent method for an ellipse.
 */
extern Datum g_sellipse_consistent(PG_FUNCTION_ARGS);

/*
 * GIST's consistent method for a box.
 */
extern Datum g_sbox_consistent(PG_FUNCTION_ARGS);

/*
 * GIST's penalty method.
 */
extern Datum g_spherekey_penalty(PG_FUNCTION_ARGS);

/*
 * GIST's picksplit method. This method is using the double sorting node
 * splitting algorithm for R-Trees. See "A new double sorting-based node
 * splitting algorithm for R-tree", A. Korotkov.
 */
extern Datum g_spherekey_picksplit(PG_FUNCTION_ARGS);

extern Datum pointkey_in(PG_FUNCTION_ARGS);
extern Datum pointkey_out(PG_FUNCTION_ARGS);
extern Datum pointkey_volume(PG_FUNCTION_ARGS);
extern Datum pointkey_area(PG_FUNCTION_ARGS);
extern Datum pointkey_perimeter(PG_FUNCTION_ARGS);
extern Datum g_spoint3_compress(PG_FUNCTION_ARGS);
extern Datum g_spoint3_union(PG_FUNCTION_ARGS);
extern Datum g_spoint3_same(PG_FUNCTION_ARGS);
extern Datum g_spoint3_consistent(PG_FUNCTION_ARGS);
extern Datum g_spoint3_penalty(PG_FUNCTION_ARGS);
extern Datum g_spoint3_picksplit(PG_FUNCTION_ARGS);
extern Datum g_spoint3_distance(PG_FUNCTION_ARGS);
extern Datum g_spoint3_fetch(PG_FUNCTION_ARGS);

#endif
