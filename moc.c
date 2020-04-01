/* this goes in front to detect the chealpix API break */
#include "pgs_moc.h"

#include <stddef.h>
#include <string.h>
#include <access/gin.h>

#include "circle.h"
#include "polygon.h"

PG_FUNCTION_INFO_V1(smoc_in);
PG_FUNCTION_INFO_V1(smoc_out);
PG_FUNCTION_INFO_V1(moc_debug);
PG_FUNCTION_INFO_V1(set_smoc_output_type);
PG_FUNCTION_INFO_V1(smoc_info);
PG_FUNCTION_INFO_V1(smoc_area);
PG_FUNCTION_INFO_V1(smoc_order);
PG_FUNCTION_INFO_V1(smoc_eq);
PG_FUNCTION_INFO_V1(smoc_neq);
PG_FUNCTION_INFO_V1(smoc_overlap);
PG_FUNCTION_INFO_V1(smoc_overlap_neg);
PG_FUNCTION_INFO_V1(smoc_subset_smoc);
PG_FUNCTION_INFO_V1(smoc_subset_smoc_neg);
PG_FUNCTION_INFO_V1(smoc_superset_smoc);
PG_FUNCTION_INFO_V1(smoc_superset_smoc_neg);

PG_FUNCTION_INFO_V1(healpix_subset_smoc);
PG_FUNCTION_INFO_V1(healpix_not_subset_smoc);
PG_FUNCTION_INFO_V1(smoc_superset_healpix);
PG_FUNCTION_INFO_V1(smoc_not_superset_healpix);
PG_FUNCTION_INFO_V1(spoint_subset_smoc);
PG_FUNCTION_INFO_V1(spoint_not_subset_smoc);
PG_FUNCTION_INFO_V1(smoc_superset_spoint);
PG_FUNCTION_INFO_V1(smoc_not_superset_spoint);
PG_FUNCTION_INFO_V1(smoc_union);
PG_FUNCTION_INFO_V1(smoc_intersection);
PG_FUNCTION_INFO_V1(smoc_degrade);
PG_FUNCTION_INFO_V1(smoc_spoint);
PG_FUNCTION_INFO_V1(smoc_disc);
PG_FUNCTION_INFO_V1(smoc_scircle);
PG_FUNCTION_INFO_V1(smoc_spoly);

PG_FUNCTION_INFO_V1(smoc_gin_extract_value);
PG_FUNCTION_INFO_V1(smoc_gin_extract_value_fine);
PG_FUNCTION_INFO_V1(smoc_gin_extract_query);
PG_FUNCTION_INFO_V1(smoc_gin_extract_query_fine);
PG_FUNCTION_INFO_V1(smoc_gin_consistent);

int32 smoc_output_type = 0;

#define LAYDEB 0

#define DEBUG_(code) do { if (LAYDEB) { code; } } while (0);
#define DEBUG_CLEAR DEBUG_(moc_debug_clear(moc_error_out))
#define DEBUG_OUT(str) DEBUG_(moc_debug_log(moc_error_out, str))
#define DEBUG_LN DEBUG_OUT("\n")
#define DEBUG_PRINT(name, fmt) DEBUG_(moc_debug_log(moc_error_out, \
													#name " = " fmt "; ", name))
#define DEBUG_INT(name) DEBUG_PRINT(name, "%d")
#define DEBUG_64(name) DEBUG_PRINT(name, MOC_FORMAT_64)
// #define DEBUG_(name) DEBUG_PRINT(name, "%")

#define DEBUG_ENTRY(name) DEBUG_(hpint64 x; \
									memmove(&x, name->start, HP64_SIZE); \
									moc_debug_log(moc_error_out, "*" #name \
										" = {" MOC_FORMAT_64 " -> %d}; ", \
										x, name->offset))
#define DEBUG_INTERVAL(name) DEBUG_(moc_debug_log(moc_error_out, "*" #name \
													" = [" MOC_FORMAT_64 ", \
													" MOC_FORMAT_64 "); ", \
													name->first, name->second))
int32
moc_mod_floor(int32 x, int32 mod)
{
	return x - x % mod;
}

int32
moc_tree_entry_floor(int32 x)
{
	return moc_mod_floor(x, MOC_TREE_ENTRY_SIZE);
}

int32
moc_interval_floor(int32 x)
{
	return moc_mod_floor(x, MOC_INTERVAL_SIZE);
}

static void
moc_error_out(const char *message, int type)
{
	ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
			errmsg("MOC processing error: %s", message)));
}

static bool
index_invalid(hpint64 npix, long index)
{
	return index < 0 || index >= npix;
}

static int
dbg_to_moc(int pos, void* moc_in_context, long order, hpint64 first, hpint64 last,
												pgs_error_handler error_out)
{
	add_to_moc(moc_in_context, order, first, last, error_out);
	return 0;
}

// The release_moc_in_context() calls of smoc_in() should be eventually wrapped
// into the finalisation function of a proper PostgreSQL memory context.
//
Datum
smoc_in(PG_FUNCTION_ARGS)
{
	char*	input_text = PG_GETARG_CSTRING(0);
	char	c;
	Smoc*	moc;
	void*	moc_in_context = create_moc_in_context(moc_error_out);
	int32	moc_size;

	long	order = -1;
	hpint64	npix = 0;
	hpint64	nb;
	hpint64	nb2;

	int ind = 0;
	while (input_text[ind] != '\0')
	{
		nb = readNumber(input_text, &ind);
		c = readChar(input_text, &ind);

		if (c == '/') /* nb is a Healpix order */
		{
			if (nb == -1)
			{
				release_moc_in_context(moc_in_context, moc_error_out);
				ereport(ERROR, (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
						errmsg("[c.%d] Incorrect MOC syntax: a Healpix level "
								"is expected before a / character.", ind - 1),
						errhint("Expected syntax: '{healpix_order}/"
								"{healpix_index}[,...] ...', where "
								"{healpix_order} is between 0 and 29. Example: "
								"'1/0 2/3,5-10'.")));
			}
			else if (order_invalid((int) nb))
			{
				release_moc_in_context(moc_in_context, moc_error_out);
				ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
						errmsg("[c.%d] Incorrect Healpix order " MOC_FORMAT_64
															".", ind - 1, nb),
						errhint("A valid Healpix order must be an integer "
								"between 0 and 29.")));
			}
			order = nb;
			npix = c_npix(order);
		}
		else if (c == ',') /* nb is a Healpix index */
		{
			if (index_invalid(npix, nb))
			{
				release_moc_in_context(moc_in_context, moc_error_out);
				ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
							errmsg("[c.%d] Incorrect Healpix index " MOC_FORMAT_64 ".",
																ind - 1, nb),
							errhint("At order %ld, a Healpix index must be "
									"an integer between 0 and " MOC_FORMAT_64 ".", order,
									 								npix - 1)));
			}
			dbg_to_moc(1, moc_in_context, order, nb, nb + 1, moc_error_out);
		}
		else if (c == '-')  /* next Healpix number must follow */
		{
			nb2 = readNumber(input_text, &ind);
			if (nb2 == -1)
			{
				release_moc_in_context(moc_in_context, moc_error_out);
				ereport(ERROR, (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
						errmsg("[c.%d] Incorrect MOC syntax: a second Healpix "
								"index is expected after a '-' character.",
								(ind - 1)),
						errhint("Expected syntax: '{healpix_order}/"
								"{healpix_index}[,...] ...', where "
								"{healpix_order} is between 0 and 29. Example: "
								"'1/0 2/3,5-10'.")));
			}

			c = readChar(input_text, &ind);
			if (isdigit(c))
			{
				ind--;
			}

			if (nb == -1)
			{
				release_moc_in_context(moc_in_context, moc_error_out);
				ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
							errmsg("[c.%d] Healpix order must not be negative.",
							ind - 1),
							errhint("Expected syntax: '{healpix_order}/"
								"{healpix_index}[,...] ...', where "
								"{healpix_order} is between 0 and 29. Example: "
								"'1/0 2/3,5-10'.")));
			}
			if (index_invalid(npix, nb))
			{
				release_moc_in_context(moc_in_context, moc_error_out);
				ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
							errmsg("[c.%d] Incorrect Healpix index " MOC_FORMAT_64 ".",
																ind - 1, nb),
							errhint("At order %ld, a Healpix index must be "
									"an integer between 0 and " MOC_FORMAT_64 ".", order,
									 								npix - 1)));
			}
			if (index_invalid(npix, nb2))
			{
				release_moc_in_context(moc_in_context, moc_error_out);
				ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
							errmsg("[c.%d] Incorrect Healpix index " MOC_FORMAT_64 ".",
																ind - 1, nb2),
							errhint("At order %ld, a Healpix index must be "
									"an integer between 0 and " MOC_FORMAT_64 ".", order,
									 								npix - 1)));
			}
			if (nb >= nb2)
			{
				release_moc_in_context(moc_in_context, moc_error_out);
				ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
						errmsg("[c.%d] Incorrect Healpix range " MOC_FORMAT_64 "-" MOC_FORMAT_64 ".",
															ind - 1 , nb, nb2),
						errhint("The first value of a range (here " MOC_FORMAT_64 ") must be"
								" less than the second one (here " MOC_FORMAT_64 ").",
								nb, nb2)));
			}
			dbg_to_moc(2, moc_in_context, order, nb, nb2 + 1, moc_error_out);
		}
		else if (isdigit(c)) /* nb is the last Healpix index of this level */
		{
			if (index_invalid(npix, nb))
			{
				release_moc_in_context(moc_in_context, moc_error_out);
				ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
							errmsg("[c.%d] Incorrect Healpix index " MOC_FORMAT_64 ".",
																ind - 1, nb),
							errhint("At order %ld, a Healpix index must be "
									"an integer between 0 and " MOC_FORMAT_64 ".", order,
									 								npix - 1)));
			}
			ind--; /* Nothing else to do in this function */
			dbg_to_moc(3, moc_in_context, order, nb, nb + 1, moc_error_out);
		}
		else if (c == '\0') /* nb should be the last Healpix index */
		{
			if (order == -1)
			{
				release_moc_in_context(moc_in_context, moc_error_out);
				ereport(ERROR, (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				errmsg("Incorrect MOC syntax: empty string found."),
				errhint("The minimal expected syntax is: '{healpix_order}/', "
						"where {healpix_order} must be an integer between 0 and"
						" 29. This will create an empty MOC. Example: '1/'.")));
			}
			else if (nb != -1 && index_invalid(npix, nb))
			{
				release_moc_in_context(moc_in_context, moc_error_out);
				ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
							errmsg("[c.%d] Incorrect Healpix index " MOC_FORMAT_64 ".",
																ind - 1, nb),
							errhint("At order %ld, a Healpix index must be "
									"an integer between 0 and " MOC_FORMAT_64 ".", order,
									 								npix - 1)));
			}
			else
			{
				dbg_to_moc(4, moc_in_context, order, nb, nb + 1, moc_error_out);
			}
		}
		else
		{
			release_moc_in_context(moc_in_context, moc_error_out);
			ereport(ERROR, (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
					errmsg("[c.%d] Incorrect MOC syntax: unsupported character "
							"'%c'.", ind - 1, c),
					errhint("Expected syntax: '{healpix_order}/"
							"{healpix_index}[,...] ...', where {healpix_order} "
							"is between 0 and 29. Example: '1/0 2/3,5-10'.")));
		}
	}

	moc_size = VARHDRSZ + get_moc_size(moc_in_context, moc_error_out);
	/* palloc() will leak the moc_in_context if it fails :-/ */
	moc = (Smoc*) palloc0(moc_size);
	SET_VARSIZE(moc, moc_size);
	create_moc_release_context(moc_in_context, moc, moc_error_out);
	PG_RETURN_POINTER(moc);
}

/**
 * [readNumber(char*, int*) --> long]
 * 
 * Read the next character as a number.
 * 
 * All whitespaces from the given position to the first digit are silently
 * skiped.
 * 
 * If the first non-whitespace character is not a digit, this function returns
 * immediately -1. Otherwise, all successive digits are read and gather to build
 * the corresponding number.
 * 
 * The given index is incremented each time a character (whatever it is) is
 * skiped.
 * 
 * @param mocAscii  The string in which the number must be read.
 * @param start     Index from which the number must be read.
 *                  This value is incremented at each read character.
 * 
 * @return  The read number,
 *          or -1 if no digit has been found.
 */
hpint64 readNumber(const char* mocAscii, int* start)
{
  hpint64 nb = -1;
  
  // Skip all space characters:
  while(mocAscii[*start] != '\0' && isspace(mocAscii[*start]))
    (*start)++;
  
  /* If the current character is a digit,
   * read all other possible successive digits: */
  if (mocAscii[*start] != '\0' && isdigit(mocAscii[*start])){
    nb = 0;
    do{
      nb = nb*10 + (long)(mocAscii[*start]-48);
      (*start)++;
    }while(isdigit(mocAscii[*start]) && mocAscii[*start] != '\0');
  }
  
  // Return the read number:
  return nb;
}

/**
 * [readChar(char*, int*) --> char]
 * 
 * Read the next non-whitespace character.
 * 
 * All whitespaces from the given position are silently skipped.
 * 
 * The given index is incremented each time a character (whatever it is) is
 * skipped.
 * 
 * When the end of the given string is reached, this function stops immediately
 * and returns '\0'. The given index is then set to the corresponding position.
 * 
 * @param mocAscii  The string in which the next non-whitespace character must
 *                  be read.
 * @param start     Index from which the character must be read.
 *                  This value is incremented at each read character.
 * 
 * @return  The read non-whitespace character,
 *          or '\0' if the end has been reached.
 */
char readChar(const char* mocAscii, int* start)
{
  // Skip all space characters:
  while(mocAscii[*start] != '\0' && isspace(mocAscii[*start]))
    (*start)++;
  
  // Return the read character:
  if (mocAscii[*start] == '\0')
    return '\0';
  else
    return mocAscii[(*start)++];
}

Datum
set_smoc_output_type(PG_FUNCTION_ARGS)
{
	int32	out_type = PG_GETARG_INT32(0);
	char	*buffer	= (char *) palloc(80);
	if (out_type < 0)
		out_type = 0;
	if (out_type > 1)
		out_type = 1;
	smoc_output_type = out_type;
	switch (smoc_output_type)
	{
		case 0:
			sprintf(buffer, "Set output type to MOC-ASCII (0).");
			break;
		case 1:
			sprintf(buffer, "Set output type to MOC intervals (1).");
			break;
		default:
			moc_error_out("set_smoc_output_type()", 0);
	}
	PG_RETURN_CSTRING(buffer);
}

Datum
smoc_info(PG_FUNCTION_ARGS)
{
	/* get just the MOC header: */
	Smoc *moc = (Smoc *) PG_DETOAST_DATUM_SLICE(PG_GETARG_DATUM(0), 0,
															MOC_HEADER_VARSIZE);
	char *p = psprintf("version: %u, order: %u, depth: %u, first: " MOC_FORMAT_64U
			", last: " MOC_FORMAT_64U ", area: " MOC_FORMAT_64U ", tree_begin: %d, data_begin: %d",
			moc->version, moc->order, moc-> depth, moc->first, moc->last, moc->area,
			moc->tree_begin, moc->data_begin);
	PG_RETURN_TEXT_P(cstring_to_text(p));
}

Datum
smoc_area(PG_FUNCTION_ARGS)
{
	/* get just the MOC header: */
	Smoc *moc = (Smoc *) PG_DETOAST_DATUM_SLICE(PG_GETARG_DATUM(0), 0,
															MOC_HEADER_VARSIZE);
	PG_RETURN_INT64(moc->area);
}

Datum
smoc_order(PG_FUNCTION_ARGS)
{
	/* get just the MOC header: */
	Smoc *moc = (Smoc *) PG_DETOAST_DATUM_SLICE(PG_GETARG_DATUM(0), 0,
															MOC_HEADER_VARSIZE);
	PG_RETURN_INT32(moc->order);
}

Datum
smoc_out(PG_FUNCTION_ARGS)
{
	Smoc *moc = (Smoc *) PG_DETOAST_DATUM(PG_GETARG_DATUM(0));
	moc_out_data out_context
		= create_moc_out_context(moc, VARSIZE(moc) - VARHDRSZ, moc_error_out);
	/* palloc() will leak the out_context if it fails :-/ */
	char *buf = (char *) palloc(out_context.out_size);
	print_moc_release_context(out_context, buf, moc_error_out);
	PG_RETURN_CSTRING(buf);
}

Datum
moc_debug(PG_FUNCTION_ARGS)
{
	const char *c_str;
	size_t x_size = get_moc_debug(&c_str, moc_error_out);
	const char *x = (const char*) palloc(x_size);
	memmove((void*) x, (void*) c_str, x_size);
	PG_RETURN_TEXT_P(cstring_to_text(x));
}

static bool
smoc_eq_impl(Datum a, Datum b)
{
	Smoc*	moc_a = (Smoc *)PG_DETOAST_DATUM_SLICE(a, 0, MOC_HEADER_PAGE);
	Smoc*	moc_b = (Smoc *)PG_DETOAST_DATUM_SLICE(b, 0, MOC_HEADER_PAGE);
	int32	moc_a_end;
	int32	moc_b_end;
	char*	moc_a_base;
	char*	moc_b_base;

	/* quick exit if header values don't agree */
	if (moc_a->order != moc_b->order || moc_a->first != moc_b->first || moc_a->last != moc_b->last || moc_a->area != moc_b->area)
		return false;
	if (moc_a->data_begin != moc_b->data_begin) /* this needs to be reconsidered if the MOC version is updated */
		return false;

	/* get full moc */
	moc_a = (Smoc *)PG_DETOAST_DATUM(a);
	moc_b = (Smoc *)PG_DETOAST_DATUM(b);

	moc_a_end = VARSIZE(moc_a) - VARHDRSZ;
	moc_b_end = VARSIZE(moc_b) - VARHDRSZ;
	moc_a_base = MOC_BASE(moc_a);
	moc_b_base = MOC_BASE(moc_b);

	if (moc_a_end != moc_b_end) /* this needs to be reconsidered if the MOC version is updated */
		return false;

	for (int j = moc_a->data_begin; j < moc_a_end; j += MOC_INTERVAL_SIZE) // iterate over both in parallel
	{
		moc_interval *x;
		moc_interval *y;
		// page bumps
		int32 mod = (j + MOC_INTERVAL_SIZE) % PG_TOAST_PAGE_FRAGMENT;
		if (mod > 0 && mod < MOC_INTERVAL_SIZE)
			j += MOC_INTERVAL_SIZE - mod;
		x = MOC_INTERVAL(moc_a_base, j);
		y = MOC_INTERVAL(moc_b_base, j);

		if (x->first != y->first || x->second != y->second)
			return false;
	}

	return true;
}

Datum
smoc_eq(PG_FUNCTION_ARGS)
{
	Datum	a = PG_GETARG_DATUM(0);
	Datum	b = PG_GETARG_DATUM(1);
	PG_RETURN_BOOL(smoc_eq_impl(a, b));
}

Datum
smoc_neq(PG_FUNCTION_ARGS)
{
	Datum	a = PG_GETARG_DATUM(0);
	Datum	b = PG_GETARG_DATUM(1);
	PG_RETURN_BOOL(! smoc_eq_impl(a, b));
}

static bool
smoc_overlap_impl(Datum a, Datum b)
{
	Smoc*	moc_a = (Smoc *)PG_DETOAST_DATUM_SLICE(a, 0, MOC_HEADER_PAGE);
	Smoc*	moc_b = (Smoc *)PG_DETOAST_DATUM_SLICE(b, 0, MOC_HEADER_PAGE);
	int32	i = moc_a->data_begin;
	int32	j = moc_b->data_begin;
	int32	moc_a_end;
	int32	moc_b_end;
	char*	moc_a_base;
	char*	moc_b_base;

	// empty mocs do not overlap
	if (moc_a->area == 0 || moc_b->area == 0)
		return false;
	// quick exit if the mocs do not overlap at all
	if (moc_a->first >= moc_b->last || moc_a->last <= moc_b->first)
		return false;
	// all-sky mocs overlap everything
	if (moc_a->area == MOC_AREA_ALL_SKY || moc_b->area == MOC_AREA_ALL_SKY)
		return true;

	// get full moc
	moc_a = (Smoc *)PG_DETOAST_DATUM(a);
	moc_b = (Smoc *)PG_DETOAST_DATUM(b);

	moc_a_end = VARSIZE(moc_a) - VARHDRSZ;
	moc_b_end = VARSIZE(moc_b) - VARHDRSZ;
	moc_a_base = MOC_BASE(moc_a);
	moc_b_base = MOC_BASE(moc_b);

	while (i < moc_a_end && j < moc_b_end) // iterate over both in parallel
	{
		moc_interval *x;
		moc_interval *y;

		// page bumps
		int32 mod = (i + MOC_INTERVAL_SIZE) % PG_TOAST_PAGE_FRAGMENT;
		if (mod > 0 && mod < MOC_INTERVAL_SIZE)
			i += MOC_INTERVAL_SIZE - mod;
		x = MOC_INTERVAL(moc_a_base, i);

		mod = (j + MOC_INTERVAL_SIZE) % PG_TOAST_PAGE_FRAGMENT;
		if (mod > 0 && mod < MOC_INTERVAL_SIZE)
			j += MOC_INTERVAL_SIZE - mod;
		y = MOC_INTERVAL(moc_b_base, j);

		if (x->second <= y->first) // a entirely left of b, advance a
		{
			i += MOC_INTERVAL_SIZE;
			continue;
		}
		if (y->second <= x->first) // b entirely left of a, advance b
		{
			j += MOC_INTERVAL_SIZE;
			continue;
		}

		return true; // entries overlap
	}

	return false;
}

Datum
smoc_overlap(PG_FUNCTION_ARGS)
{
	Datum	a = PG_GETARG_DATUM(0);
	Datum	b = PG_GETARG_DATUM(1);
	PG_RETURN_BOOL(smoc_overlap_impl(a, b));
}

Datum
smoc_overlap_neg(PG_FUNCTION_ARGS)
{
	Datum	a = PG_GETARG_DATUM(0);
	Datum	b = PG_GETARG_DATUM(1);
	PG_RETURN_BOOL(! smoc_overlap_impl(a, b));
}

/* check if moc_a is a subset of moc_b */
static bool
smoc_subset_impl(Datum a, Datum b)
{
	Smoc*	moc_a = (Smoc *)PG_DETOAST_DATUM_SLICE(a, 0, MOC_HEADER_PAGE);
	Smoc*	moc_b = (Smoc *)PG_DETOAST_DATUM_SLICE(b, 0, MOC_HEADER_PAGE);
	int32	i = moc_a->data_begin;
	int32	j = moc_b->data_begin;
	int32	moc_a_end;
	int32	moc_b_end;
	char*	moc_a_base;
	char*	moc_b_base;

	// an empty moc is subset of all mocs
	if (moc_a->area == 0)
		return true;
	// b is the whole sky
	if (moc_b->area == MOC_AREA_ALL_SKY)
		return true;
	// a cannot be larger than b
	if (moc_a->area > moc_b->area)
		return false;

	// quick exit if the mocs do not overlap at all
	if (moc_a->first >= moc_b->last || moc_a->last <= moc_b->first)
		return false;

	// get full moc
	moc_a = (Smoc *)PG_DETOAST_DATUM(a);
	moc_b = (Smoc *)PG_DETOAST_DATUM(b);

	moc_a_end = VARSIZE(moc_a) - VARHDRSZ;
	moc_b_end = VARSIZE(moc_b) - VARHDRSZ;
	moc_a_base = MOC_BASE(moc_a);
	moc_b_base = MOC_BASE(moc_b);

	while (i < moc_a_end) // iterate over a
	{
		int32 mod;
		moc_interval *x;
		moc_interval *y;

		// end of b reached while there's still 'a' intervals
		if (j >= moc_b_end)
			return false;

		// page bumps
		mod = (i + MOC_INTERVAL_SIZE) % PG_TOAST_PAGE_FRAGMENT;
		if (mod > 0 && mod < MOC_INTERVAL_SIZE)
			i += MOC_INTERVAL_SIZE - mod;
		x = MOC_INTERVAL(moc_a_base, i);

		mod = (j + MOC_INTERVAL_SIZE) % PG_TOAST_PAGE_FRAGMENT;
		if (mod > 0 && mod < MOC_INTERVAL_SIZE)
			j += MOC_INTERVAL_SIZE - mod;
		y = MOC_INTERVAL(moc_b_base, j);

		// advance b until as long as we are before the 'a' interval
		if (y->second <= x->first)
		{
			j += MOC_INTERVAL_SIZE;
			continue;
		}

		// we can check for a match now
		if (x->first < y->first) // a starts left of b
			return false;
		if (x->second > y->second) // b ends right of a
			return false;

		// advance interval that has the lowest end
		if (x->second == y->second)
		{
			i += MOC_INTERVAL_SIZE;
			j += MOC_INTERVAL_SIZE;
		}
		else if (x->second <= y->second)
			i += MOC_INTERVAL_SIZE;
		else
			j += MOC_INTERVAL_SIZE;
		// TODO: we could walk the tree structure to find the next interesting interval
	}

	return true;
}

Datum
smoc_subset_smoc(PG_FUNCTION_ARGS)
{
	Datum	a = PG_GETARG_DATUM(0);
	Datum	b = PG_GETARG_DATUM(1);
	PG_RETURN_BOOL(smoc_subset_impl(a, b));
}

Datum
smoc_subset_smoc_neg(PG_FUNCTION_ARGS)
{
	Datum	a = PG_GETARG_DATUM(0);
	Datum	b = PG_GETARG_DATUM(1);
	PG_RETURN_BOOL(! smoc_subset_impl(a, b));
}

Datum
smoc_superset_smoc(PG_FUNCTION_ARGS)
{
	Datum	a = PG_GETARG_DATUM(0);
	Datum	b = PG_GETARG_DATUM(1);
	PG_RETURN_BOOL(smoc_subset_impl(b, a));
}

Datum
smoc_superset_smoc_neg(PG_FUNCTION_ARGS)
{
	Datum	a = PG_GETARG_DATUM(0);
	Datum	b = PG_GETARG_DATUM(1);
	PG_RETURN_BOOL(! smoc_subset_impl(b, a));
}

static bool
entry_equal(moc_tree_entry *a, hpint64 y)
{
	hpint64 x;
	memmove(&x, a->start, HP64_SIZE);
	return x == y;
}

static void
fit_level_end_to(int32 d_end, int32 *level_end, int32 level_begin, int32 mod)
{
	if (*level_end > d_end)
		*level_end = level_begin + moc_mod_floor(d_end - level_begin, mod);
}

bool
healpix_subset_smoc_impl(hpint64 x, Datum y)
{
	int32 end = toast_raw_datum_size(y) - VARHDRSZ;
	Smoc *moc;
	char *moc_base;
	int32 tree_begin;
	int32 depth;
	int32 *level_ends;
	int32 k;
	int32 d_begin = 0;
	int32 d_end;
	int32 level_begin;
	int32 level_end;
	moc_tree_entry *first;
	moc_tree_entry *last;
	moc_tree_entry *w;
	moc_interval *first_i;
	moc_interval *last_i;
	moc_interval *v;
	int32 count;

	if (end == MIN_MOC_SIZE) /* should include empty root node... */
		return false;
	/* get the first page -- it contains at least the full root node */
	moc = (Smoc *) PG_DETOAST_DATUM_SLICE(y, 0, MOC_HEADER_PAGE);
	d_end = VARSIZE(moc) - VARHDRSZ; /* the end of the actually detoasted part */
	if (moc->first == moc->last || x < moc->first || x >= moc->last)
		return false;

	moc_base = MOC_BASE(moc);
	tree_begin = moc->tree_begin;

	depth = moc->depth;
	level_ends = (int32 *)(moc_base + tree_begin);
	level_begin = tree_begin + 4 * depth; 

	for (k = 0; k < depth; ++k)
	{
		level_end = level_ends[k];
		fit_level_end_to(d_end, &level_end, level_begin, MOC_TREE_ENTRY_SIZE);
		first =	MOC_ENTRY(moc_base, level_begin	- d_begin);
		last =	MOC_ENTRY(moc_base, level_end	- d_begin);

		w = entry_lower_bound(first, last, x);

		if (w != last && entry_equal(w, x))
			return true;
		--w;
		level_begin = w->offset;
		if (level_begin < d_end)
			continue;
		/* get the (potentially fractional) page the entry points to */
		d_begin = level_begin;
		count = PG_TOAST_PAGE_FRAGMENT - d_begin % PG_TOAST_PAGE_FRAGMENT;
		moc_base = (char *) PG_DETOAST_DATUM_SLICE(y, d_begin, count);
		d_end = d_begin + VARSIZE(moc_base) - VARHDRSZ;
		moc_base += VARHDRSZ;
	}
	// search in interval page defined by (moc_base, level_begin, d_end)
	fit_level_end_to(d_end, &end, level_begin, MOC_INTERVAL_SIZE);
	first_i	= MOC_INTERVAL(moc_base, level_begin - d_begin);
	last_i	= MOC_INTERVAL(moc_base, end - d_begin);

//debug: print out the whole of [*first_i, *last_i) -- also for nodes!!!
	v = interval_lower_bound(first_i, last_i, x);

	
	if (v != last_i && v->first == x)
		return true;
	--v;
	return v->first < x && x < v->second;
}

Datum
healpix_subset_smoc_c(hpint64 x, Datum y) 
{
	PG_RETURN_BOOL(healpix_subset_smoc_impl(x, y));
}

Datum
healpix_not_subset_smoc_c(hpint64 x, Datum y) 
{
	PG_RETURN_BOOL(!healpix_subset_smoc_impl(x, y));
}

bool
spoint_subset_smoc_impl(SPoint *x, Datum y)
{
	return healpix_subset_smoc_impl(healpix_nest_c(29, x), y);
}

Datum
spoint_subset_smoc_c(SPoint *x, Datum y)
{
	PG_RETURN_BOOL(spoint_subset_smoc_impl(x, y));
}

Datum
spoint_not_subset_smoc_c(SPoint *x, Datum y) 
{
	PG_RETURN_BOOL(!spoint_subset_smoc_impl(x, y));
}

Datum
healpix_subset_smoc(PG_FUNCTION_ARGS)
{
	hpint64	x = PG_GETARG_INT64(0);
	Datum	y = PG_GETARG_DATUM(1);
	return healpix_subset_smoc_c(x, y);
}

Datum
healpix_not_subset_smoc(PG_FUNCTION_ARGS)
{
	hpint64	x = PG_GETARG_INT64(0);
	Datum	y = PG_GETARG_DATUM(1);
	return healpix_not_subset_smoc_c(x, y);
}

Datum
smoc_superset_healpix(PG_FUNCTION_ARGS)
{
	Datum	x = PG_GETARG_DATUM(0);
	hpint64	y = PG_GETARG_INT64(1);
	return healpix_subset_smoc_c(y, x);
}

Datum
smoc_not_superset_healpix(PG_FUNCTION_ARGS)
{
	Datum	x = PG_GETARG_DATUM(0);
	hpint64	y = PG_GETARG_INT64(1);
	return healpix_not_subset_smoc_c(y, x);
}

Datum
spoint_subset_smoc(PG_FUNCTION_ARGS)
{
	SPoint *x = (SPoint *) PG_GETARG_POINTER(0);
	Datum	y = PG_GETARG_DATUM(1);
	return spoint_subset_smoc_c(x, y);
}

Datum
spoint_not_subset_smoc(PG_FUNCTION_ARGS)
{
	SPoint *x = (SPoint *) PG_GETARG_POINTER(0);
	Datum	y = PG_GETARG_DATUM(1);
	return spoint_not_subset_smoc_c(x, y);
}

Datum
smoc_superset_spoint(PG_FUNCTION_ARGS)
{
	Datum	x = PG_GETARG_DATUM(0);
	SPoint *y = (SPoint *) PG_GETARG_POINTER(1);
	return spoint_subset_smoc_c(y, x);
}

Datum
smoc_not_superset_spoint(PG_FUNCTION_ARGS)
{
	Datum	x = PG_GETARG_DATUM(0);
	SPoint *y = (SPoint *) PG_GETARG_POINTER(1);
	return spoint_not_subset_smoc_c(y, x);
}

Datum
smoc_union(PG_FUNCTION_ARGS)
{
	Smoc*	moc_a = (Smoc *) PG_DETOAST_DATUM(PG_GETARG_DATUM(0));
	Smoc*	moc_b = (Smoc *) PG_DETOAST_DATUM(PG_GETARG_DATUM(1));
	Smoc*	moc_ret;
	void*	moc_in_context = create_moc_in_context(moc_error_out);
	int32	moc_size;

	moc_union(moc_in_context, moc_a, VARSIZE(moc_a) - VARHDRSZ, moc_b, VARSIZE(moc_b) - VARHDRSZ, moc_error_out);

	moc_size = VARHDRSZ + get_moc_size(moc_in_context, moc_error_out);
	/* palloc() will leak the moc_in_context if it fails :-/ */
	moc_ret = (Smoc*) palloc0(moc_size);
	SET_VARSIZE(moc_ret, moc_size);
	create_moc_release_context(moc_in_context, moc_ret, moc_error_out);
	PG_RETURN_POINTER(moc_ret);
}

Datum
smoc_intersection(PG_FUNCTION_ARGS)
{
	Smoc*	moc_a = (Smoc *) PG_DETOAST_DATUM(PG_GETARG_DATUM(0));
	Smoc*	moc_b = (Smoc *) PG_DETOAST_DATUM(PG_GETARG_DATUM(1));
	Smoc*	moc_ret;
	void*	moc_in_context = create_moc_in_context(moc_error_out);
	int32	moc_size;

	moc_intersection(moc_in_context, moc_a, VARSIZE(moc_a) - VARHDRSZ, moc_b, VARSIZE(moc_b) - VARHDRSZ, moc_error_out);

	moc_size = VARHDRSZ + get_moc_size(moc_in_context, moc_error_out);
	/* palloc() will leak the moc_in_context if it fails :-/ */
	moc_ret = (Smoc*) palloc0(moc_size);
	SET_VARSIZE(moc_ret, moc_size);
	create_moc_release_context(moc_in_context, moc_ret, moc_error_out);
	PG_RETURN_POINTER(moc_ret);
}

Datum
smoc_degrade(PG_FUNCTION_ARGS)
{
	int		order = PG_GETARG_INT32(0);
	Smoc*	moc_a = (Smoc *) PG_DETOAST_DATUM(PG_GETARG_DATUM(1));
	Smoc*	moc_ret;
	void*	moc_in_context = create_moc_in_context(moc_error_out);
	int32	moc_size;

	check_order(order);

	moc_round(moc_in_context, order, moc_a, VARSIZE(moc_a) - VARHDRSZ, moc_error_out);

	moc_size = VARHDRSZ + get_moc_size(moc_in_context, moc_error_out);
	/* palloc() will leak the moc_in_context if it fails :-/ */
	moc_ret = (Smoc*) palloc0(moc_size);
	SET_VARSIZE(moc_ret, moc_size);
	create_moc_release_context(moc_in_context, moc_ret, moc_error_out);
	PG_RETURN_POINTER(moc_ret);
}

Datum
smoc_spoint(PG_FUNCTION_ARGS)
{
	int		order = PG_GETARG_INT32(0);
	SPoint* p = (SPoint*) PG_GETARG_POINTER(1);
	hpint64 pixel, first, last;
	void*	moc_in_context;
	int32	moc_size;
	Smoc*	moc_ret;

	check_order(order);

	pixel = healpix_nest_c(order, p);
	first = c_healpix_convert_nest(pixel, order, HEALPIX_MAX_ORDER);
	last = c_healpix_convert_nest(pixel + 1, order, HEALPIX_MAX_ORDER);

	moc_in_context = create_moc_in_context(moc_error_out);
	moc_healpix(moc_in_context, first, last, moc_error_out);

	moc_size = VARHDRSZ + get_moc_size(moc_in_context, moc_error_out);
	moc_ret = (Smoc*) palloc0(moc_size);
	SET_VARSIZE(moc_ret, moc_size);

	create_moc_release_context(moc_in_context, moc_ret, moc_error_out);
	PG_RETURN_POINTER(moc_ret);
}

Datum
smoc_disc(PG_FUNCTION_ARGS)
{
	int		order = PG_GETARG_INT32(0);
	double	lng = PG_GETARG_FLOAT8(1);
	double	lat = PG_GETARG_FLOAT8(2);
	double	radius = PG_GETARG_FLOAT8(3);
	void*	moc_in_context;
	int32	moc_size;
	Smoc*	moc_ret;

	check_order(order);

	moc_in_context = create_moc_in_context(moc_error_out);
	moc_disc(moc_in_context, order, lng, lat, radius, moc_error_out);

	moc_size = VARHDRSZ + get_moc_size(moc_in_context, moc_error_out);
	moc_ret = (Smoc*) palloc0(moc_size);
	SET_VARSIZE(moc_ret, moc_size);

	create_moc_release_context(moc_in_context, moc_ret, moc_error_out);
	PG_RETURN_POINTER(moc_ret);
}

Datum
smoc_scircle(PG_FUNCTION_ARGS)
{
	int		order = PG_GETARG_INT32(0);
	SCIRCLE *c = (SCIRCLE *) PG_GETARG_POINTER(1);
	void*	moc_in_context;
	int32	moc_size;
	Smoc*	moc_ret;

	check_order(order);

	moc_in_context = create_moc_in_context(moc_error_out);
	moc_disc(moc_in_context, order, c->center.lat, c->center.lng, c->radius, moc_error_out);

	moc_size = VARHDRSZ + get_moc_size(moc_in_context, moc_error_out);
	moc_ret = (Smoc*) palloc0(moc_size);
	SET_VARSIZE(moc_ret, moc_size);

	create_moc_release_context(moc_in_context, moc_ret, moc_error_out);
	PG_RETURN_POINTER(moc_ret);
}

Datum
smoc_spoly(PG_FUNCTION_ARGS)
{
	int		order = PG_GETARG_INT32(0);
	SPOLY*	polygon = (SPOLY *) PG_GETARG_SPOLY(1);
	void*	moc_in_context;
	int32	moc_size;
	Smoc*	moc_ret;

	check_order(order);

	moc_in_context = create_moc_in_context(moc_error_out);
	moc_polygon(moc_in_context, order, polygon->npts, (float8 *)polygon->p, moc_error_out);

	moc_size = VARHDRSZ + get_moc_size(moc_in_context, moc_error_out);
	moc_ret = (Smoc*) palloc0(moc_size);
	SET_VARSIZE(moc_ret, moc_size);

	create_moc_release_context(moc_in_context, moc_ret, moc_error_out);
	PG_RETURN_POINTER(moc_ret);
}

/* GIN index ***********************************/

static Datum
smoc_gin_extract_internal(Smoc *moc_a, int32 *nkeys, int gin_order)
{
	char*	moc_a_base = MOC_BASE(moc_a);
	int32	moc_a_end = VARSIZE(moc_a) - VARHDRSZ;
	int32	nalloc = 4;
	Datum*	keys = palloc(nalloc * sizeof(Datum));

	*nkeys = 0;

	for (int32 a = moc_a->data_begin; a < moc_a_end; a = next_interval(a))
	{
		moc_interval *x = MOC_INTERVAL(moc_a_base, a);

		int		shift = 2 * (HEALPIX_MAX_ORDER - gin_order); // degrade to MOC_GIN_ORDER
		int32	first = (x->first >> shift); // set low bits to zero
		hpint64	low_bits_one = (1L << shift) - 1;
		int32	second = ((x->second + low_bits_one) >> shift); // round low bits up
		Assert(shift > 32); // internal GIN datatype isn't 64 bits

		// split interval into individual pixels of order MOC_GIN_ORDER
		for (int32 p = first; p < second; p++)
		{
			if (*nkeys > 0 && keys[*nkeys - 1] == p) // has (larger) pixel already been added?
				continue;
			if (*nkeys >= nalloc)
			{
				nalloc *= 2;
				Assert(nalloc < 2000000);
				keys = repalloc(keys, nalloc * sizeof(Datum));
			}
			keys[(*nkeys)++] = Int32GetDatum(p);
		}
	}

	PG_RETURN_POINTER(keys);
}

Datum
smoc_gin_extract_value(PG_FUNCTION_ARGS)
{
	Smoc*	moc_a = (Smoc *) PG_DETOAST_DATUM(PG_GETARG_DATUM(0));
	int32*	nkeys = (int32 *) PG_GETARG_POINTER(1);

	PG_RETURN_DATUM(smoc_gin_extract_internal(moc_a, nkeys, MOC_GIN_ORDER));
}

Datum
smoc_gin_extract_value_fine(PG_FUNCTION_ARGS)
{
	Smoc*	moc_a = (Smoc *) PG_DETOAST_DATUM(PG_GETARG_DATUM(0));
	int32*	nkeys = (int32 *) PG_GETARG_POINTER(1);

	PG_RETURN_DATUM(smoc_gin_extract_internal(moc_a, nkeys, MOC_GIN_ORDER_FINE));
}

Datum
smoc_gin_extract_query(PG_FUNCTION_ARGS)
{
	Smoc*	moc_a = (Smoc *) PG_DETOAST_DATUM(PG_GETARG_DATUM(0));
	int32*	nkeys = (int32 *) PG_GETARG_POINTER(1);
	StrategyNumber st = PG_GETARG_UINT16(2);
	int32*	searchmode = (int32 *) PG_GETARG_POINTER(6);

	if (st == MOC_GIN_STRATEGY_SUBSET)
		*searchmode = GIN_SEARCH_MODE_INCLUDE_EMPTY;

	PG_RETURN_DATUM(smoc_gin_extract_internal(moc_a, nkeys, MOC_GIN_ORDER));
}

Datum
smoc_gin_extract_query_fine(PG_FUNCTION_ARGS)
{
	Smoc*	moc_a = (Smoc *) PG_DETOAST_DATUM(PG_GETARG_DATUM(0));
	int32*	nkeys = (int32 *) PG_GETARG_POINTER(1);
	StrategyNumber st = PG_GETARG_UINT16(2);
	int32*	searchmode = (int32 *) PG_GETARG_POINTER(6);

	if (st == MOC_GIN_STRATEGY_SUBSET)
		*searchmode = GIN_SEARCH_MODE_INCLUDE_EMPTY;

	PG_RETURN_DATUM(smoc_gin_extract_internal(moc_a, nkeys, MOC_GIN_ORDER_FINE));
}

Datum
smoc_gin_consistent(PG_FUNCTION_ARGS)
{
	bool*	check = (bool *) PG_GETARG_POINTER(0);
	StrategyNumber st = PG_GETARG_UINT16(1);
	//Smoc*	moc_a = (Smoc *) PG_DETOAST_DATUM(PG_GETARG_DATUM(2));
	//int32	moc_a_end = VARSIZE(moc_a) - VARHDRSZ;
	int32	nkeys = PG_GETARG_INT32(3);
	bool*	recheck = (bool *) PG_GETARG_POINTER(5);

	switch (st)
	{
		case MOC_GIN_STRATEGY_INTERSECTS:
			/* return true if we have any overlap */
			for (int i = 0; i < nkeys; i++)
			{
				if (check[i])
				{
					*recheck = true;
					PG_RETURN_BOOL(true);
				}
			}

			PG_RETURN_BOOL(false);

		case MOC_GIN_STRATEGY_SUBSET:
			/* defer decision to recheck */
			*recheck = true;
			PG_RETURN_BOOL(true);

		case MOC_GIN_STRATEGY_SUPERSET:
			/* return true when all pixels are contained in the indexed value */
			for (int i = 0; i < nkeys; i++)
			{
				if (! check[i])
				{
					PG_RETURN_BOOL(false);
				}
			}

			*recheck = true;
			PG_RETURN_BOOL(true);

		default:
			Assert(0);
	}

	/* not reached */
	PG_RETURN_NULL();
}
