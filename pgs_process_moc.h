#ifndef __PGS_PROCESS_MOC_H__
#define __PGS_PROCESS_MOC_H__

#ifdef __cplusplus
extern "C" {
#endif

/* this goes in front to detect the chealpix API break */
#include "pgs_chealpix.h"

#include <c.h>					/* PostgreSQL type definitions */

/*
 * PG_TOAST_PAGE_FRAGMENT should equal TOAST_MAX_CHUNK_SIZE, but we
 * do not want to include too much stuff just for the small gain of
 * getting its value right. Usually, TOAST_MAX_CHUNK_SIZE equals 1996.
 * 
 * #include <postgres.h>
 * #include <access/tuptoaster.h>	// for TOAST_MAX_CHUNK_SIZE
 */

extern int32 smoc_output_type;

typedef void (*pgs_error_handler)(const char*, int);

#define HP64_SIZE (sizeof(hpint64))
#define PG_TOAST_PAGE_FRAGMENT 1996 /* not divisible by HP64_SIZE == 8 */

/* moc_interval: an half-open interval [first, last) of Healpix elements */

typedef struct
{
	hpint64 first;
	hpint64 second;
} moc_interval;

typedef struct
{
	int32	offset;				/* counts in units of char, from start of MOC */
	char	start[HP64_SIZE];
} moc_tree_entry;

#define MOC_INDEX_ALIGN (sizeof(int32))

#define MOC_INTERVAL_SIZE (sizeof(moc_interval))
#define MOC_TREE_ENTRY_SIZE (sizeof(moc_tree_entry))
#define MOC_LEAF_PAGE_LEN (PG_TOAST_PAGE_FRAGMENT / MOC_INTERVAL_SIZE)
#define MOC_TREE_PAGE_LEN (PG_TOAST_PAGE_FRAGMENT / MOC_TREE_ENTRY_SIZE)

/*
 * this particular layout should prevent the compiler from introducing unwanted
 * padding
 */
typedef struct
{
	char		vl_len_[4];	/* size of PostgreSQL variable-length data */
	uint16		version;	/* version of the 'toasty' MOC data structure */
	uint8		order;		/* actual MOC order */
	uint8		depth;		/* depth of B+-tree */
	hpint64		first;		/* first Healpix index in set */
	hpint64		last;		/* 1 + (last Healpix index in set) */
	hpint64		area;		/* number of covered Healpix cells */
	int32		tree_begin;	/* start of B+ tree, past the options block */
	int32		data_begin;	/* start of Healpix intervals, bypassing the tree */
	int32		data[1];	/* no need to optimise for empty MOCs */
} Smoc;

#define MOC_HEADER_VARSIZE (offsetof(Smoc, data))
#define MOC_HEADER_SIZE (MOC_HEADER_VARSIZE - VARHDRSZ)
#define MIN_MOC_SIZE (sizeof(Smoc) - VARHDRSZ)

#define MOC_HEADER_PAGE  ((PG_TOAST_PAGE_FRAGMENT) > (MOC_HEADER_VARSIZE) \
						? (PG_TOAST_PAGE_FRAGMENT) : (MOC_HEADER_VARSIZE))

#define MOC_BASE(moc) ((char*) &(moc->version))
#define MOC_ENTRY(moc_base, offset) ((moc_tree_entry *)(moc_base + offset))
#define MOC_INTERVAL(moc_base, offset) ((moc_interval *)(moc_base + offset))

#define MOC_MAX_OPTIONS_SIZE 0 /* no options yet */

int32
moc_mod_floor(int32, int32);
int32
moc_tree_entry_floor(int32);
int32
moc_interval_floor(int32);

void*
create_moc_in_context(pgs_error_handler);

void
release_moc_in_context(void*, pgs_error_handler);

void
add_to_moc(void*, long, hpint64, hpint64, pgs_error_handler);

int
get_moc_size(void*, pgs_error_handler);

int
create_moc_release_context(void*, Smoc*, pgs_error_handler);

typedef struct
{
	void*	context;
	size_t	out_size;
} moc_out_data;

moc_out_data
create_moc_out_context(Smoc*, int32, pgs_error_handler);

/* for the final smoc_out() using a proper Postgres memory context */
void 
release_moc_out_context(moc_out_data, pgs_error_handler);

void
print_moc_release_context(moc_out_data, char*, pgs_error_handler);

size_t
get_moc_debug(const char**, pgs_error_handler);

void
moc_debug_log(pgs_error_handler, const char *, ...)
#ifdef __GNUC__
__attribute__((format(printf, 2, 3)))
#endif
;

void
moc_debug_clear(pgs_error_handler);

moc_tree_entry*
entry_lower_bound(moc_tree_entry*, moc_tree_entry*, hpint64);

moc_interval*
interval_lower_bound(moc_interval*, moc_interval*, hpint64);

#ifdef __cplusplus
}
#endif

#endif
