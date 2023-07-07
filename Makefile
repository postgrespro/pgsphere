PGSPHERE_VERSION = 1.2.2
EXTENSION        = pg_sphere
RELEASE_SQL      = $(EXTENSION)--$(PGSPHERE_VERSION).sql
USE_PGXS         = 1
USE_HEALPIX      =? 1

# the base dir name may be changed depending on git clone command
SRC_DIR = $(shell basename $(shell pwd))

MODULE_big = pg_sphere
OBJS       = src/sscan.o src/sparse.o src/sbuffer.o src/vector3d.o src/point.o \
             src/euler.o src/circle.o src/line.o src/ellipse.o src/polygon.o \
             src/path.o src/box.o src/output.o src/gq_cache.o src/gist.o \
             src/key.o src/gnomo.o src/epochprop.o

ifneq ($(USE_HEALPIX),0)
OBJS      += src/healpix.o src/moc.o src/process_moc.o \
             healpix_bare/healpix_bare.o
endif

DATA_built  = $(RELEASE_SQL) \
			  pg_sphere--unpackaged--1.1.5beta0gavo.sql \
			  pg_sphere--1.0--1.0_gavo.sql \
			  pg_sphere--1.0_gavo--1.1.5beta0gavo.sql \
			  pg_sphere--1.1.5beta0gavo--1.1.5beta2gavo.sql \
			  pg_sphere--1.1.5beta2gavo--1.1.5beta4gavo.sql \
			  pg_sphere--1.1.5beta4gavo--1.2.0.sql \
			  pg_sphere--1.2.0--1.2.1.sql \
			  pg_sphere--1.2.1--1.2.2.sql

DOCS        = README.pg_sphere COPYRIGHT.pg_sphere
REGRESS     = init tables points euler circle line ellipse poly path box index \
              contains_ops contains_ops_compat bounding_box_gist gnomo epochprop

ifneq ($(USE_HEALPIX),0)
REGRESS    += healpix moc mocautocast
endif

REGRESS_9_5 = index_9.5 # experimental for spoint3

TESTS       = init_test tables points euler circle line ellipse poly path box \
              index contains_ops contains_ops_compat bounding_box_gist gnomo \
              epochprop

ifneq ($(USE_HEALPIX),0)
TESTS      += healpix moc mocautocast
endif

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
              pgs_gist.sql gnomo.sql

ifneq ($(USE_HEALPIX),0)
PGS_SQL    += healpix.sql
endif

PGS_SQL    += pgs_gist_spoint3.sql

ifneq ($(USE_HEALPIX),0)
PGS_SQL    += pgs_moc_type.sql pgs_moc_compat.sql pgs_moc_ops.sql \
              pgs_moc_geo_casts.sql
endif

PGS_SQL    += pgs_epochprop.sql

PGS_SQL_9_5 = pgs_9.5.sql # experimental for spoint3

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

# experimental for spoint3
pg_version := $(word 2,$(shell $(PG_CONFIG) --version))
pg_version_9_5_plus = $(if $(filter-out 9.1% 9.2% 9.3% 9.4%,$(pg_version)),y,n)
has_explain_summary = $(if $(filter-out 9.%,$(pg_version)),y,n)

## the use of spoint 3 is too experimental and preliminary:
#ifeq ($(pg_version_9_5_plus),y)
#	REGRESS += $(REGRESS_9_5)
#	TESTS   += $(REGRESS_9_5)
#	PGS_SQL += $(PGS_SQL_9_5)
#endif

crushtest: REGRESS += $(CRUSH_TESTS)
crushtest: installcheck

ifneq ($(USE_HEALPIX),0)
ifeq ($(has_explain_summary),y)
        REGRESS += moc1 moc100
endif
endif

ifeq ($(pg_version_9_5_plus),y)
        PGS_TMP_DIR = --temp-instance=tmp_check
else
        PGS_TMP_DIR = --temp-install=tmp_check --top-builddir=test_top_build_dir
endif

test: pg_sphere.test.sql sql/init_test.sql
	cp expected/init_test.out.in expected/init_test.out
ifneq ($(USE_HEALPIX),0)
	cat expected/init_test_healpix.out.in >> expected/init_test.out
endif
	$(pg_regress_installcheck) $(PGS_TMP_DIR) $(REGRESS_OPTS) $(TESTS)

pg_sphere.test.sql: $(RELEASE_SQL) $(shlib)
	tail -n+3 $< | sed 's,MODULE_PATHNAME,$(realpath $(shlib)),g' >$@


$(RELEASE_SQL): $(addsuffix .in, $(RELEASE_SQL) $(PGS_SQL))
	cat $^ > $@

# for "create extension from unpacked*":

UPGRADE_UNP_COMMON = pgs_types.sql pgs_point.sql pgs_euler.sql pgs_circle.sql \
	pgs_line.sql pgs_ellipse.sql pgs_polygon.sql pgs_path.sql \
	pgs_box.sql pgs_contains_ops_compat.sql pgs_gist.sql \
	pgs_gist_contains_ops.sql contains-ops-fixes-1.sql

AUGMENT_UNP_COMMON = upgrade_scripts/pgs_pre111.sql pgs_contains_ops.sql \
	gnomo.sql
# for vanilla 1.1.1 users:
AUGMENT_UNP_111 = $(AUGMENT_UNP_COMMON) pgs_gist_pointkey.sql

# for 1.1.2+ users: 'from unpacked_1.1.2plus'
AUGMENT_UNP_FOR_112plus = $(AUGMENT_UNP_COMMON)
UPGRADE_UNP_FOR_112plus = pgs_gist_pointkey.sql pgs_gist_drop_spoint2.sql.in

# for "alter extension":

# TODO: add dynamic pl/pgsql to do perform an additional
#    "ALTER EXTENSION pg_sphere UPDATE TO '1.1.5_from_before_2016-02-07';"
# if required.
#
# default 1.0 (after 2016-02-07) -> 1.1.5
UPGRADE_1_0_PRE_xxxxxx = contains-ops-fixes-2.sql
# '1.1.5_from_2015-08-31'
AUGMENT_1_0_PRE_AAF2D5 = pgs_contains_ops.sql gnomo.sql
UPGRADE_1_0_PRE_AAF2D5 = contains-ops-fixes-1.sql pgs_gist_drop_spoint2.sql.in \
						pgs_gist_contains_ops.sql

# vanilla 'create from unpackaged' must assume 1.1.1
# ...

# create "create extension from unpacked*" files

# create "alter extension" files


ifeq ($(pg_version_9_5_plus),y)
# 1.1.1.5 -> 1.1.5.1 for Postgres 9.5+ features
else
endif

# local stuff follows here
AUGMENT_GAVO_111 = $(AUGMENT_UNP_111) # for vanilla 1.1.1 users
ifneq ($(USE_HEALPIX),0)
AUGMENT_GAVO_111 += healpix.sql
endif
UPGRADE_GAVO_111 = $(UPGRADE_UNP_COMMON)

# add new HEALPix functions and experimental spoint3
ifneq ($(USE_HEALPIX),0)
AUGMENT_FROM_GAVO = healpix.sql
endif
AUGMENT_FROM_GAVO += pgs_gist_spoint3.sql

AUGMENT_UNP_115B0G = $(AUGMENT_UNP_111) $(AUGMENT_FROM_GAVO)
UPGRADE_UNP_115B0G = $(UPGRADE_UNP_COMMON)

AUGMENT_1_0_115B0G = $(AUGMENT_FROM_GAVO)
UPGRADE_1_0_115B0G = contains-ops-fixes-2.sql pgs_gist_drop_spoint2.sql

# test installation 0
pg_sphere--unpackaged--1.1.5beta0gavo.sql: $(addsuffix .in, \
		$(AUGMENT_GAVO_111) \
		$(addprefix upgrade_scripts/, $(UPGRADE_GAVO_111)))
	cat upgrade_scripts/$@.in $^ > $@

# (The upgrade of test installation A has been completed.)

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

pg_sphere--1.1.5beta2gavo--1.1.5beta4gavo.sql: pgs_moc_compat.sql.in
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
