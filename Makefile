
include Makefile.common.mk

RELEASE_SQL      = $(EXTENSION)--$(PGSPHERE_VERSION).sql
USE_PGXS         = 1
USE_HEALPIX     ?= 1
PGINDENT        ?= pgindent
PGBSDINDENT     ?= pg_bsd_indent

# the base dir name may be changed depending on git clone command
SRC_DIR = $(shell basename $(shell pwd))

MODULE_big = pg_sphere
OBJS       = src/sscan.o src/sparse.o src/sbuffer.o src/vector3d.o src/point.o \
             src/euler.o src/circle.o src/circle_sel.o src/line.o src/ellipse.o src/polygon.o \
             src/path.o src/box.o src/output.o src/gq_cache.o src/gist.o src/gist_support.o \
             src/key.o src/gnomo.o src/epochprop.o src/brin.o

ifneq ($(USE_HEALPIX),0)
OBJS      += src/healpix.o src/moc.o src/process_moc.o \
             healpix_bare/healpix_bare.o
endif

DATA_built  = $(RELEASE_SQL) \
			  pg_sphere--1.0--1.0_gavo.sql \
			  pg_sphere--1.0_gavo--1.1.5beta0gavo.sql \
			  pg_sphere--1.1.5beta0gavo--1.1.5beta2gavo.sql \
			  pg_sphere--1.1.5beta2gavo--1.1.5beta4gavo.sql \
			  pg_sphere--1.1.5beta4gavo--1.2.0.sql \
			  pg_sphere--1.2.0--1.2.1.sql \
			  pg_sphere--1.2.1--1.2.2.sql \
			  pg_sphere--1.2.2--1.2.3.sql \
			  pg_sphere--1.2.3--1.3.0.sql \
			  pg_sphere--1.3.0--1.3.1.sql \
			  pg_sphere--1.3.1--1.3.2.sql

DOCS        = README.pg_sphere COPYRIGHT.pg_sphere
TESTS       = tables points euler circle line ellipse poly path box \
              index contains_ops contains_ops_compat bounding_box_gist gnomo \
              epochprop contains overlaps spoint_brin sbox_brin selectivity
REGRESS     = init $(TESTS)

PG_CFLAGS	+= -DPGSPHERE_VERSION=$(PGSPHERE_VERSION)
PG_CPPFLAGS	+= -DPGSPHERE_VERSION=$(PGSPHERE_VERSION)

ifndef CXXFLAGS
# no support for CXXFLAGS in PGXS before v11
CXXFLAGS = -Wall -Wpointer-arith -Wendif-labels \
		-Wmissing-format-attribute -Wformat-security -g -O2 -fPIC
endif

EXTRA_CLEAN = $(PGS_SQL) pg_sphere.test.sql

CRUSH_TESTS = init_extended circle_extended

# order of sql files is important
PGS_SQL     = pgs_types.sql pgs_point.sql pgs_euler.sql pgs_circle.sql \
              pgs_line.sql pgs_ellipse.sql pgs_polygon.sql pgs_path.sql \
              pgs_box.sql pgs_contains_ops.sql pgs_contains_ops_compat.sql \
              pgs_gist.sql gnomo.sql pgs_brin.sql pgs_circle_sel.sql pgs_hash.sql

ifneq ($(USE_HEALPIX),0)
TESTS      += healpix moc moc1 moc100 mocautocast
PGS_SQL    += healpix.sql
endif

PGS_SQL    += pgs_gist_spoint3.sql

ifneq ($(USE_HEALPIX),0)
PGS_SQL    += pgs_moc_type.sql pgs_moc_ops.sql \
              pgs_moc_geo_casts.sql
endif

PGS_SQL    += pgs_epochprop.sql

ifdef USE_PGXS
  ifndef PG_CONFIG
    PG_CONFIG = pg_config
  endif
  PGXS := $(shell $(PG_CONFIG) --pgxs)
  include $(PGXS)
else
  subdir = contrib/pg_sphere
  top_builddir = ../..
  PG_CONFIG := $(top_builddir)/src/bin/pg_config/pg_config
  include $(top_builddir)/src/Makefile.global
  include $(top_srcdir)/contrib/contrib-global.mk
endif

ifneq ($(USE_HEALPIX),0)
# compiler settings for linking with libhealpix_cxx
PKG_CONFIG ?= pkg-config
override CPPFLAGS += $(shell $(PKG_CONFIG) --cflags healpix_cxx)
SHLIB_LINK += $(shell $(PKG_CONFIG) --libs healpix_cxx)
LINK.shared = g++ -shared
endif

# healpix_bare.c isn't ours so we refrain from fixing the warnings in there
healpix_bare/healpix_bare.o : healpix_bare/healpix_bare.c
	$(COMPILE.c) -Wno-declaration-after-statement -o $@ $^

pg_version := $(word 2,$(shell $(PG_CONFIG) --version))
has_support_functions = $(if $(filter-out 9.% 10.% 11.%,$(pg_version)),y,n)
has_index_options = $(if $(filter-out 9.% 10.% 11.% 12.%,$(pg_version)),y,n)

crushtest: TESTS += $(CRUSH_TESTS)
crushtest: installcheck

ifeq ($(has_support_functions),y)
PGS_SQL    += pgs_gist_support.sql
TESTS      += gist_support
endif

ifneq ($(USE_HEALPIX),0)
ifeq ($(has_index_options),y)
PGS_SQL    += pgs_moc_options.sql
TESTS      += moc_options
endif
endif

# "make test" uses a special initialization file that doesn't rely on "create extension"
test: pg_sphere.test.sql
	$(pg_regress_installcheck) --temp-instance=tmp_check $(REGRESS_OPTS) init_test $(TESTS)

pg_sphere.test.sql: $(RELEASE_SQL) $(shlib)
	tail -n+3 $< | sed 's,MODULE_PATHNAME,$(realpath $(shlib)),g' >$@

$(RELEASE_SQL): pg_sphere_head.sql.in $(addsuffix .in, $(PGS_SQL))
	cat $^ > $@

# default 1.0 (after 2016-02-07) -> 1.1.5
UPGRADE_1_0_PRE_xxxxxx = contains-ops-fixes-2.sql
# '1.1.5_from_2015-08-31'
AUGMENT_1_0_PRE_AAF2D5 = pgs_contains_ops.sql gnomo.sql
UPGRADE_1_0_PRE_AAF2D5 = contains-ops-fixes-1.sql pgs_gist_drop_spoint2.sql.in \
						pgs_gist_contains_ops.sql

# add new HEALPix functions and experimental spoint3
ifneq ($(USE_HEALPIX),0)
AUGMENT_FROM_GAVO = healpix.sql
endif
AUGMENT_FROM_GAVO += pgs_gist_spoint3.sql

AUGMENT_1_0_115B0G = $(AUGMENT_FROM_GAVO)
UPGRADE_1_0_115B0G = contains-ops-fixes-2.sql pgs_gist_drop_spoint2.sql

# test installation B (generic)
pg_sphere--1.0--1.0_gavo.sql: # dummy upgrade to allow for descriptive names
	cat upgrade_scripts/$@.in > $@
pg_sphere--1.0_gavo--1.1.5beta0gavo.sql: $(addsuffix .in, \
		$(AUGMENT_1_0_115B0G) \
		$(addprefix upgrade_scripts/, $(UPGRADE_1_0_115B0G)))
	cat upgrade_scripts/$@.in $^ > $@

ifneq ($(USE_HEALPIX),0)
pg_sphere--1.1.5beta0gavo--1.1.5beta2gavo.sql: pgs_moc_type.sql.in
	cat upgrade_scripts/$@.in $^ > $@

pg_sphere--1.1.5beta2gavo--1.1.5beta4gavo.sql:
	cat upgrade_scripts/$@.in $^ > $@

pg_sphere--1.1.5beta4gavo--1.2.0.sql: pgs_moc_ops.sql.in
	cat upgrade_scripts/$@.in $^ > $@

pg_sphere--1.2.0--1.2.1.sql: pgs_moc_geo_casts.sql.in pgs_epochprop.sql.in
	cat upgrade_scripts/$@.in $^ > $@

pg_sphere--1.2.1--1.2.2.sql: upgrade_scripts/pg_sphere--1.2.1--1.2.2-healpix.sql.in
	cat upgrade_scripts/$@.in $^ > $@
else
pg_sphere--1.1.5beta0gavo--1.1.5beta2gavo.sql:
	cat upgrade_scripts/$@.in > $@

pg_sphere--1.1.5beta2gavo--1.1.5beta4gavo.sql:
	cat upgrade_scripts/$@.in > $@

pg_sphere--1.1.5beta4gavo--1.2.0.sql:
	cat upgrade_scripts/$@.in > $@

pg_sphere--1.2.0--1.2.1.sql: pgs_epochprop.sql.in
	cat upgrade_scripts/$@.in $^ > $@

pg_sphere--1.2.1--1.2.2.sql:
	cat upgrade_scripts/$@.in > $@
endif

pg_sphere--1.2.2--1.2.3.sql:
	cat upgrade_scripts/$@.in > $@

pg_sphere--1.2.3--1.3.0.sql: pgs_brin.sql.in
	cat upgrade_scripts/$@.in $^ > $@

pg_sphere--1.3.0--1.3.1.sql:
	cat upgrade_scripts/$@.in > $@

ifeq ($(has_support_functions),y)
pg_sphere--1.3.1--1.3.2.sql: pgs_gist_support.sql.in
endif
ifneq ($(USE_HEALPIX),0)
ifeq ($(has_index_options),y)
pg_sphere--1.3.1--1.3.2.sql: pgs_moc_options.sql.in
endif
endif
pg_sphere--1.3.1--1.3.2.sql: pgs_circle_sel.sql.in pgs_hash.sql.in
	cat upgrade_scripts/$@.in $^ > $@

# end of local stuff

src/sscan.o : src/sparse.c

src/sparse.c: src/sparse.y
ifdef YACC
	$(YACC) -d $(YFLAGS) -p sphere_yy -o $@ $<
else
	@$(missing) bison $< $@
endif

src/sscan.c : src/sscan.l
ifdef FLEX
	$(FLEX) $(FLEXFLAGS) -Psphere -o$@ $<
else
	@$(missing) flex $< $@
endif

dist : clean sparse.c sscan.c
	find . -name '*~' -type f -exec rm {} \;
	cd .. && tar --transform s/$(SRC_DIR)/pgsphere-$(PGSPHERE_VERSION)/ --exclude CVS --exclude .git -czf pgsphere-$(PGSPHERE_VERSION).tar.gz $(SRC_DIR) && cd -

# To use pgindent, set the PATH environment variable to include the directories
# containing the binaries pgindent and pg_bsd_indent. It is important to
# utilize a specific version of pg_bsd_indent, which sources can be found
# in the <postgresql>/src/tools/pg_bsd_indent directory, where <postgresql>
# is the root directory of the PostgreSQL project source tree.
#
# The sources of the utilities can be found in the following directories:
# - <postgresql>/src/tools/pgindent
# - <postgresql>/src/tools/pg_bsd_indent
#
# pgindent-typedefs.list should be updated every time after implementing
# new types and introducing new typedefs in the code. For details how
# to update pgindent-typedefs.list and for other information about pgindent,
# please, read the doc: <postgresql>/src/tools/pgindent/README.
#
# pgindent-excludes.list is used to specify files to be ignored.
#
pgindent:
	$(PGINDENT) \
		--typedefs=pgindent-typedefs.list \
		--excludes=pgindent-excludes.list \
		--indent=${PGBSDINDENT} \
		src
