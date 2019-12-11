#include <cstddef>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <cstdarg>
#include <map>
#include <vector>
#include <algorithm>
#include <ostream>
#include <iostream>
#include <string>
#include <sstream>
#include <exception>
#include <stdexcept>

#include "pgs_process_moc.h"

#define LAYDEB 0

#define DEBUG_(code) do { if (LAYDEB) { code; } } while (0);
#define DEBUG_LOG(msg) DEBUG_(log_string() += msg)
#define DEBUG_LF DEBUG_LOG("\n")
#define DEBUG_SHOW(name) (to_string(#name " = ") + to_string(name) + "; ")
#define DEBUG_DX(name) DEBUG_LOG(DEBUG_SHOW(name))
#define DEBUG_MA(name) m.addln(to_string("_" #name " = ") + to_string(name));

// PGS_TRY / PGS_CATCH: use an additional 'do {} while (0);' to allow for
// 'break;' as an alternative to 'return;'

#define PGS_TRY try	{ do {
#define PGS_CATCH 	} while (0); }			\
	catch (std::exception & e)				\
	{										\
		delete p;							\
		error_out(e.what(), 0);				\
	}										\
	catch (...)								\
	{										\
		delete p;							\
		error_out("unknown exception", 0);	\
	}

using std::size_t;

static std::string &
log_string()
{
	static std::string s;
	return s;
}

size_t
get_moc_debug(const char** c_str, pgs_error_handler error_out)
{
	int* p = 0;
	size_t size;
	PGS_TRY
		*c_str = log_string().c_str();
		size = 1 + log_string().size();
	PGS_CATCH
	return size;
}

void
moc_debug_log(pgs_error_handler error_out, const char *fm, ...)
{
	int* p = 0;
	char buffer[2048];
	PGS_TRY
		va_list arguments;
		va_start(arguments, fm);
		vsprintf(buffer, fm, arguments);
		va_end(arguments);
		log_string().append(buffer);
	PGS_CATCH
}

void
moc_debug_clear(pgs_error_handler error_out)
{
	int* p = 0;
	PGS_TRY
		log_string().clear();
	PGS_CATCH
}

// Throwing expections from destructors is not strictly forbidden, it is just
// discouraged in the strongest possible way.
template <class C>
void
release_context(void* context, pgs_error_handler error_out)
{
	C* p = static_cast<C*>(context);
	try
	{
		delete p;
	}
	catch (std::exception & e)
	{
		error_out(e.what(), 0);
	}
	catch (...)
	{
		error_out("unknown exception", 0);
	}
}

char* data_as_char(Smoc* moc, size_t offset = 0)
{
	return offset + reinterpret_cast<char*>(&((moc->data)[0]));
}

static
char* detoasted_offset(Smoc* moc, size_t offset = 0)
{
	return offset + reinterpret_cast<char*>(&(moc->version));
}

template<class X, class Y>
static
X* data_as(Y* y)
{
	return reinterpret_cast<X*>(y);
}

static
size_t align_round(size_t offset, size_t alignment)
{
	return (1 + offset / alignment) * alignment;
}

static void
healpix_convert(hpint64 & idx, int32 from_order)
{
	idx <<= (29 - from_order) * 2;
}

moc_interval
make_interval(hpint64 first, hpint64 last)
{
	moc_interval x;
	x.first = first;
	x.second = last;
	return x;
}

template<class X>
moc_tree_entry
make_node(int32 offset, const X & start)
{
	moc_tree_entry x;
	x.offset = offset;
	std::memmove(x.start, &start, HP64_SIZE);
	return x;
}

bool
operator<(const moc_interval & x, const moc_interval & y)
{
	return x.first < y.first;
}

typedef std::map<hpint64, hpint64>		moc_map;
typedef moc_map::iterator				map_iterator;
typedef moc_map::const_iterator			map_const_iter;
typedef moc_map::const_reverse_iterator	map_rev_iter;
typedef moc_map::value_type				moc_map_entry;

template<class X>
std::ostream &
interval_out(std::ostream & os, const X & x)
{
	os << "[" << x.first << ", " << x.second << ")";
	return os;
}

std::ostream &
operator<<(std::ostream & os, const moc_interval & x)
{
	return interval_out(os, x);
}

std::ostream &
operator<<(std::ostream & os, const moc_map_entry & x)
{
	return interval_out(os, x);
}

std::ostream &
operator<<(std::ostream & os, const moc_tree_entry & x)
{
	hpint64 start;
	std::memmove(&start, x.start, HP64_SIZE);
	os << "{" << start << " -> " << x.offset << "}";
	return os;
}

template<class X>
std::string to_string(const X & x)
{
	std::ostringstream oss;
	oss << x;
	return oss.str();
}

struct moc_output
{
	std::string s;
};

struct moc_tree_layout
{
	size_t entries;		// # of all entries of a particular level
	size_t level_end;	// index of next entity below this level
	moc_tree_layout(): entries(0), level_end(0) {}
	moc_tree_layout(size_t len): entries(len), level_end(0) {}
	void
	layout_level(size_t & moc_size, size_t entry_size)
	{

		// maximal # of entries in a page
		size_t page_len = PG_TOAST_PAGE_FRAGMENT / entry_size;
		// # of remaining bytes of the current page
		size_t page_rest = PG_TOAST_PAGE_FRAGMENT
											- moc_size % PG_TOAST_PAGE_FRAGMENT;
		// # of remaining entries within the current page
		size_t rest_entries = page_rest / entry_size;
		size_t rest_level = 4022250974;	// # of entries beyond the current page
		size_t this_page = 3992890810;	// # of bytes used of the current page
		if (entries >= rest_entries)
		{
			rest_level = entries - rest_entries;
			this_page = page_rest;
		}			
		else // there is only a single page fragment at this level
		{
			rest_level = 0;
			this_page = entries * entry_size;
		}

		// # of full pages the current level needs
		size_t full_pages = rest_level / page_len;
		// # of bytes that the last page, certainly fractionally, is used for
		size_t last_page = (rest_level % page_len) * entry_size;


		size_t this_page_entries = entries * entry_size;
		if (full_pages || last_page)
			this_page_entries = this_page;

		size_t full_pages_space = PG_TOAST_PAGE_FRAGMENT * full_pages;
		// special case: end of entries at end of page
		if (last_page == 0)
		{
			full_pages_space = full_pages ? page_len * entry_size : 0;
			if (full_pages > 1)
				full_pages_space += PG_TOAST_PAGE_FRAGMENT * (full_pages - 1);
		}

		moc_size += this_page_entries + full_pages_space + last_page;
		level_end = moc_size;
	}
};

typedef std::vector<moc_tree_layout> layout_vec;

struct moc_input
{
	moc_map		input_map;
	size_t		options_bytes;
	size_t		options_size;
	layout_vec	layout;

	std::string s;
	moc_input() : options_bytes(0), options_size(0)
	{
		layout.reserve(5);
	}
	void dump()
	{
		std::ostringstream oss;
		oss << (input_map.size() ? "{" : "{ ");
		for (map_iterator i = input_map.begin(); i != input_map.end(); ++i)
			oss << *i << " ";
		s.append(oss.str());
		*s.rbegin() = '}';
	}
	void lndump(const std::string & msg)
	{
		s.append("\n");
		s.append(msg + ":\n");
		dump();
	}
	void addln(const std::string & msg)
	{
		s.append("\n");
		s.append(msg);
	}
	std::string to_string(map_iterator i)
	{
		if (i == input_map.end())
			return "[END]";
		return std::string(i == input_map.begin() ? "[BEGIN]" : "")
				+ ::to_string(*i);
	}
};

template<class V, size_t page_size, size_t value_size = sizeof(V)>
class rpage_iter
{
private:
	char* base;
	int32 offset;
	static const size_t page_decrement = value_size
							+ page_size - (page_size / value_size) * value_size;
public:
	rpage_iter(): base(0), offset(0) {}
	rpage_iter(int32 index): base(0), offset(index) {}
	rpage_iter(char* b, int32 index): base(b), offset(index)
	{
		operator++(); // a simplification that fails for the general case
	}
	void set(const V & v)
	{
		std::memmove(base + offset, &v, value_size);
	}
	V operator *() const
	{
		V v;
		std::memmove(&v, base + offset, value_size);
		return v;
	}
	bool operator !=(const rpage_iter & x)
	{
		return base != x.base || offset != x.offset;
	}
	bool page_ready() const
	{
		// this may, correctly, result in a fractional page with just a single
		// entry:
		return offset % page_size == 0;
	}
	rpage_iter & operator++()
	{
		offset -= page_ready() ? page_decrement : value_size;
		return *this;
	}
	int index() const
	{
		return offset;
	}
	friend std::ostream &
	operator<<(std::ostream & os, const rpage_iter & x)
	{
		os << "/" << static_cast<const void*>(x.base) << "@: +" << x.offset
			<< "/";
		return os;
	}
};

typedef rpage_iter<moc_interval, PG_TOAST_PAGE_FRAGMENT>	rintv_iter;
typedef rpage_iter<moc_tree_entry, PG_TOAST_PAGE_FRAGMENT>	rnode_iter;

void*
create_moc_in_context(pgs_error_handler error_out)
{
	moc_input* p = 0;
	PGS_TRY
		p = new moc_input;
	PGS_CATCH
	return static_cast<void*>(p);
};

void
release_moc_in_context(void* moc_in_context, pgs_error_handler error_out)
{
	release_context<moc_input>(moc_in_context, error_out);
}

void
add_to_map(moc_map & input_map, hpint64 first, hpint64 last)
{
	map_iterator lower = input_map.lower_bound(first); // first element not less than 'first'
	map_iterator upper = input_map.upper_bound(last);  // first element greater than 'last'

	if (lower != input_map.begin())
	{
		map_iterator before = lower;
		--before; // element actually less than 'first'
		if (before->second >= first)
		{
			if (before->second >= last)
				return; // [first, last) \subset [before]
			lower = before;
			first = lower->first;
		}
	}
	if (upper != input_map.begin())
	{
		map_iterator after = upper;
		--after;
		if (after->second > last)
			last = after->second;
	}
	// Skip erase if it would do nothing in order to be able to use
	// an input hint for set::insert().
	// This path would be superflous with C++11's erase(), as that returns
	// the correct hint for the insert() of the general case down below.
	// The input hint lower == upper always refers the interval completely
	// past the one to insert, or to input_map.end()
	moc_map_entry input(first, last);
	if (lower == upper)
	{
		input_map.insert(lower, input);
		return;
	}
	if (lower->first == first)
	{
		lower->second = last;
		input_map.erase(++lower, upper);
		return;
	}
	input_map.erase(lower, upper);
	input_map.insert(input);
};

void
add_to_moc(void* moc_in_context, long order, hpint64 first, hpint64 last,
												pgs_error_handler error_out)
{
	moc_input* p = static_cast<moc_input*>(moc_in_context);
	PGS_TRY
		moc_input & m = *p;
		healpix_convert(first, order); // convert to order 29
		healpix_convert(last, order);
		add_to_map(m.input_map, first, last);
	PGS_CATCH
};

// get_moc_size() prepares creation of MOC

static
// calculate the number of entries of the next-higher level
void
next_level(size_t & len, size_t entry_size)
{

	// can't split a single entry into two pages:
// 	if (len <= 1)
// 	{
// 		len = 1;
// 		return;
// 	}

	// maximal # of entries in a page of the current level
	size_t page_len = PG_TOAST_PAGE_FRAGMENT / entry_size;
	// # of full pages the current level needs
	size_t full_pages = len / page_len;
	// is there an additional fractional page?
	bool frac_page = len % page_len;

	len = full_pages + 1 + frac_page;
}

int
get_moc_size(void* moc_in_context, pgs_error_handler error_out)
{
	moc_input* p = static_cast<moc_input*>(moc_in_context);
	size_t moc_size = MOC_HEADER_SIZE;
	PGS_TRY
		moc_input & m = *p;


		m.options_size = 0; // align_round(m.options_bytes, MOC_INDEX_ALIGN);
		moc_size += m.options_size;
		if (m.options_size > MOC_MAX_OPTIONS_SIZE)
			throw std::logic_error("options larger than MOC_MAX_OPTIONS_SIZE");

		// Before doing the layout, calculate the maximal size that the B+-tree
		// needs:
		// first, calculate the maximal size the interval pages take
		size_t len = m.input_map.size();
		// take upper bound of depth into account for space of level ends
		int32 moc_root_page_calc = moc_tree_entry_floor(std::ceil(
				moc_mod_floor(PG_TOAST_PAGE_FRAGMENT - moc_size, 4)
				- 3 * MOC_TREE_ENTRY_SIZE
				- 4 * (1 + std::log(1 + (1 + len /
									moc_interval_floor(PG_TOAST_PAGE_FRAGMENT)
													* PG_TOAST_PAGE_FRAGMENT))
							/ std::log(MOC_TREE_PAGE_LEN - 2))));
		size_t moc_root_page_rest = moc_root_page_calc;
		if (moc_root_page_calc < 0
								|| moc_root_page_rest < 2 * MOC_TREE_ENTRY_SIZE)
			throw std::logic_error("PG_TOAST_PAGE_FRAGMENT too small for MOCs");
		m.layout.push_back(len);

		next_level(len, MOC_INTERVAL_SIZE);
		// add the maximal sizes of each tree level
		int check;
		const int b_tree_inf = 100;
		for (check = 0; check < b_tree_inf; ++check)
		{
			m.layout.push_back(len);
			if (len * MOC_TREE_ENTRY_SIZE <= moc_root_page_rest)
				break;
			next_level(len, MOC_TREE_ENTRY_SIZE);
		}
		if (check == b_tree_inf)
			throw std::logic_error("infinite loop for MOC B-tree depth");
		
		// layout: start with the section of the ends of each B+-tree level
		size_t depth = m.layout.size() - 1;
		moc_size += depth * MOC_INDEX_ALIGN;
		// layout: B+-tree layout, starting at root node
		for (unsigned k = depth; k >= 1; --k)
{		
			m.layout[k].layout_level(moc_size, MOC_TREE_ENTRY_SIZE);
}
		if (m.layout[depth].level_end > static_cast<size_t>(
								moc_tree_entry_floor(PG_TOAST_PAGE_FRAGMENT)))
			throw std::logic_error("MOC root node spilled into second page");

		// layout: intervals

		moc_size = align_round(moc_size, HP64_SIZE);		// fix up alignment

		m.layout[0].layout_level(moc_size, MOC_INTERVAL_SIZE);

		moc_size = std::max(MIN_MOC_SIZE, moc_size);
	PGS_CATCH
	return moc_size;
};

// create_moc_release_context()
// moc_in_context:	-- must be have been prepared by get_moc_size()
// moc:				-- must be allocated with a size returned by get_moc_size()
//

int
create_moc_release_context(void* moc_in_context, Smoc* moc,
													pgs_error_handler error_out)
{
	const moc_input* p = static_cast<const moc_input*>(moc_in_context);
	int ret = 1;
	PGS_TRY
		const moc_input & m = *p;

		moc->version = 0;
		// moc->version |= 1; // flag indicating options

		hpint64	area = 0;

		// this guards against  
		char* moc_data = detoasted_offset(moc, 0);

		// All levels will be filled out from end to beginning such that
		// the above level-end values stay correct.

		// process the interval pages
		hpint64	order_log = 0;
		rintv_iter	i(moc_data, m.layout[0].level_end);
		rnode_iter	n(moc_data, m.layout[1].level_end);
		// default for "empty" root node, points past the intervals:
		rintv_iter last_i(m.layout[0].level_end);
		hpint64	first = 0;
		hpint64	last = 0;
		// intervals and next-level node:
		rnode_iter last_rend;
		rnode_iter rend;
		for (map_rev_iter r	= m.input_map.rbegin(); r != m.input_map.rend();
																			++r)
		{
			first	= r->first;
			last	= r->second;
			order_log |= first;
			order_log |= last;
			area += last - first;
			if (i.page_ready())
			{
				// need to re-factor this into its own function:
				n.set(make_node(i.index(), first));
				last_rend = n;
				rend = ++n;
			}
			i.set(make_interval(first, last));
			last_i = i;
			++i;
		}
		// put start of Healpix intervals into the header
		moc->data_begin = last_i.index();
		// If the Smoc should be the empty set, still generate an "empty"
		// root node with a single moc_tree_entry:
		// its offset member will point just at the end of the Smoc,
		// with its start member duly set to zero here.
		if (!last_i.page_ready() || m.input_map.empty())
		{
			n.set(make_node(last_i.index(), first));
			last_rend = n;
			rend = ++n;
		}
		// process the tree pages of higher-order nodes:
		size_t depth = m.layout.size() - 1;
		for (size_t k = 1; k < depth; ++k)
		{
			rnode_iter z(moc_data, m.layout[k].level_end);
			rnode_iter n(moc_data, m.layout[k + 1].level_end);
			rnode_iter last_z;
			rnode_iter z_end = rend;
			for ( ; z != z_end; ++z)
			{
				if (z.page_ready())
				{
					n.set(make_node(z.index(), (*z).start));
					last_rend = n;
					rend = ++n;
				}
				last_z = z;
			}
			if (!last_z.page_ready())
			{
				n.set(make_node(last_z.index(), (*last_z).start));
				last_rend = n;
				rend = ++n;
			}
		}

		// The level-end section must be put relative to the actual beginning
		// of the root node to prevent confusing redunancies.
		int32 tree_begin = last_rend.index() - depth * MOC_INDEX_ALIGN;
		
		// fill out level-end section
		int32* level_ends = data_as<int32>(detoasted_offset(moc, tree_begin));
		moc->depth	= depth;
		for (int k = depth; k >= 1; --k)
{
			*(level_ends + depth - k) = m.layout[k].level_end;
}
		// There may be some space between the end of the options and
		// moc->tree_begin, but simple relocation of the tree is not an option
		// because of padding at the end of the pages.

		moc->tree_begin	= tree_begin;	// start of level-end section
		moc->area	= area;
		// simple linear search shift loop to calculate the moc order
		int order;
		for (order = 29; order > 0; --order, order_log >>= 2)
			if (order_log & 3)
				break;
		moc->order	= order;
		moc->first	= 0; // first Healpix index in set
		moc->last	= 0; // 1 + (last Healpix index in set)
		if (m.input_map.size())
		{
			moc->first	= m.input_map.begin()->first;
			moc->last	= m.input_map.rbegin()->second;
		}

	PGS_CATCH
	release_moc_in_context(moc_in_context, error_out);
	return ret;
};

static
moc_interval* interval_ptr(Smoc* moc, int32 offset)
{
	return data_as<moc_interval>(detoasted_offset(moc, offset));
}

typedef std::vector<moc_map> output_map;

void
order_break(output_map & outputs, const moc_interval & x, int max_order)
{
	int order;
	hpint64 mask = 0;
	mask = ~mask ^ 3;
char mask_x[88];
sprintf(mask_x, "%016" MOC_FORMAT_64X, mask);
	hpint64 first	= x.first >> 2 * (29 - max_order);
	hpint64 second = x.second >> 2 * (29 - max_order);
	for (order = max_order; order > 0; --order, first >>= 2, second >>= 2)
	{
		if (second == first)
			return;
		moc_map & output = outputs[order];
		if (second - first < 4)
		{
			add_to_map(output, first, second);
			return;
		}
		// the follwing is sort of inefficient in case the two fragments are
		// adjacent, but who cares...
		if (first & 3)
{
			add_to_map(output, first, (first + 4) & mask);
			first += 4;
}
		if (second & 3)
{
			add_to_map(output, second & mask, second);
}

	}
	if (first != second)
		add_to_map(outputs[0], first, second);
}

void
ascii_out(std::string & m_s, char* s, Smoc* moc, int32 begin, int32 end,
															int32 entry_size)
{	

	if (moc->first == moc->last)
	{
		m_s = "0/";
		return;
	}
	// moc output fiddling:
	int order = moc->order;
	m_s.reserve(end); // rough guess
	output_map outputs(1 + order);

	for (int32 j = begin; j < end; j += entry_size)
	{
		// page bumps
		int32 mod = (j + entry_size) % PG_TOAST_PAGE_FRAGMENT;
		if (mod > 0 && mod < entry_size)
			j += entry_size - mod;
		order_break(outputs, *interval_ptr(moc, j), order);
	}
	for (int k = 0; k <= order; ++k)
	{
		const moc_map & output = outputs[k];
		if (output.size())
		{
			sprintf(s, "%d/", k);
			m_s.append(s);
		}
		for (map_const_iter i = output.begin(); i != output.end(); ++i)
		{
				hpint64 first	= i->first;
				hpint64 last	= i->second - 1;
				if (first == last)
					sprintf(s, "" MOC_FORMAT_64U ",", first);
				else
					sprintf(s, "" MOC_FORMAT_64U "-" MOC_FORMAT_64U ",", first, last);
				m_s.append(s);
		}
		if (output.size())
			*m_s.rbegin() = ' ';
	}
	m_s.resize(m_s.size() - 1);
}

moc_out_data
create_moc_out_context(Smoc* moc, int32 end, pgs_error_handler error_out)
{
	moc_output* p = 0;
	size_t out_size = 0;
	PGS_TRY
		p = new moc_output;
		moc_output & m = *p;
		char s[60];
		int32 begin	= moc->data_begin;
		int32 entry_size = MOC_INTERVAL_SIZE;
		switch (smoc_output_type)
		{
			case 0:
				// output type MOC-ASCII
				ascii_out(m.s, s, moc, begin, end, entry_size);
				break;
			case 1:
				// output type MOC intervals
				m.s.reserve(end); // rough guess
				m.s.append(begin != end ? "{" : "{ ");
				for (int32 j = begin; j < end; j += entry_size)
				{
					// page bumps
					int32 mod = (j + entry_size) % PG_TOAST_PAGE_FRAGMENT;
					if (mod > 0 && mod < entry_size)
						j += entry_size - mod;
					moc_interval & x = *interval_ptr(moc, j);
					sprintf(s, "[" MOC_FORMAT_64U ", " MOC_FORMAT_64U ") ", x.first, x.second);
					m.s.append(s);
				}
				*m.s.rbegin() = '}';
				break;
			default:
				error_out("create_moc_out_context()", 0);
		}
		out_size = m.s.length() + 1;
	PGS_CATCH
	moc_out_data ret;
	ret.context = static_cast<void*>(p);
	ret.out_size = out_size;
	return ret;
}

void 
release_moc_out_context(moc_out_data out_context, pgs_error_handler error_out)
{
	release_context<moc_output>(out_context.context, error_out);
}

void
print_moc_release_context(moc_out_data out_context, char* buffer,
													pgs_error_handler error_out)
{
	moc_output* p = static_cast<moc_output*>(out_context.context);
	PGS_TRY
		moc_output & m = *p;
		std::memmove(buffer, m.s.c_str(), out_context.out_size);
	PGS_CATCH
	release_moc_out_context(out_context, error_out);
}

static
bool entry_cmp(const moc_tree_entry & a, hpint64 y)
{
	hpint64 x;
	std::memmove(&x, a.start, HP64_SIZE);
	return x < y;
}

static
bool interval_cmp(const moc_interval & x, hpint64 y)
{
	return x.first < y;
}

moc_tree_entry*
entry_lower_bound(moc_tree_entry* first, moc_tree_entry* last, hpint64 value)
{
	return std::lower_bound(first, last, value, entry_cmp);
}

moc_interval*
interval_lower_bound(moc_interval* first, moc_interval* last, hpint64 value)
{
	return std::lower_bound(first, last, value, interval_cmp);
}

void
moc_union(void* moc_in_context, Smoc* moc_a, int32 moc_a_end, Smoc* moc_b, int32 moc_b_end,
												pgs_error_handler error_out)
{
	int32 begin	= moc_a->data_begin;
	//int32 end	= VARSIZE(moc_a) - VARHDRSZ;
	int32 entry_size = MOC_INTERVAL_SIZE;
	moc_input* p = static_cast<moc_input*>(moc_in_context);
	PGS_TRY
		moc_input & m = *p;

		for (int32 j = begin; j < moc_a_end; j += entry_size)
		{
			// page bumps
			int32 mod = (j + entry_size) % PG_TOAST_PAGE_FRAGMENT;
			if (mod > 0 && mod < entry_size)
				j += entry_size - mod;
			moc_interval & x = *interval_ptr(moc_a, j);
			add_to_map(m.input_map, x.first, x.second);
		}

		begin	= moc_b->data_begin;
		//end	= VARSIZE(moc_b) - VARHDRSZ;

		for (int32 j = begin; j < moc_b_end; j += entry_size)
		{
			// page bumps
			int32 mod = (j + entry_size) % PG_TOAST_PAGE_FRAGMENT;
			if (mod > 0 && mod < entry_size)
				j += entry_size - mod;
			moc_interval & x = *interval_ptr(moc_b, j);
			add_to_map(m.input_map, x.first, x.second);
		}
	PGS_CATCH
};

void
moc_intersection(void* moc_in_context, Smoc* moc_a, int32 moc_a_end, Smoc* moc_b, int32 moc_b_end,
												pgs_error_handler error_out)
{
	int32	a = moc_a->data_begin;
	int32	b = moc_b->data_begin;
	int32	entry_size = MOC_INTERVAL_SIZE;
	moc_input* p = static_cast<moc_input*>(moc_in_context);
	PGS_TRY
		moc_input & m = *p;

		for (; a < moc_a_end && b < moc_b_end; ) // iterate over both in parallel
		{
			// page bumps
			int32 mod = (a + entry_size) % PG_TOAST_PAGE_FRAGMENT;
			if (mod > 0 && mod < entry_size)
				a += entry_size - mod;
			moc_interval & x = *interval_ptr(moc_a, a);

			mod = (b + entry_size) % PG_TOAST_PAGE_FRAGMENT;
			if (mod > 0 && mod < entry_size)
				b += entry_size - mod;
			moc_interval & y = *interval_ptr(moc_b, b);

			if (x.second <= y.first) // a entirely left of b, advance a
			{
				a += entry_size;
				continue;
			}
			if (y.second <= x.first) // b entirely left of a, advance b
			{
				b += entry_size;
				continue;
			}

			// add intersection of the two intervals we are at now
			add_to_map(m.input_map, std::max(x.first, y.first), std::min(x.second, y.second));

			if (x.second <= y.second) // advance interval that has the lowest end (there might be more overlaps)
				a += entry_size;
			else
				b += entry_size;
		}
	PGS_CATCH
}
