PGSPHERE_VERSION = 1.2.0

# the base dir name may be changed depending on git clone command
SRC_DIR = $(shell basename $(shell pwd))

MODULE_big = pg_sphere
OBJS       = sscan.o sparse.o sbuffer.o vector3d.o point.o \
             euler.o circle.o line.o ellipse.o polygon.o \
             path.o box.o output.o gq_cache.o gist.o key.o \
             gnomo.o healpix.o moc.o process_moc.o healpix_bare/healpix_bare.o

EXTENSION   = pg_sphere
RELEASE_SQL = $(EXTENSION)--1.2.0.sql
DATA_built  = $(RELEASE_SQL) \
			  pg_sphere--unpackaged--1.1.5beta0gavo.sql \
			  pg_sphere--1.0--1.0_gavo.sql \
			  pg_sphere--1.0_gavo--1.1.5beta0gavo.sql \
			  pg_sphere--1.1.5beta0gavo--1.1.5beta2gavo.sql \
			  pg_sphere--1.1.5beta2gavo--1.1.5beta4gavo.sql \
			  pg_sphere--1.1.5beta4gavo--1.2.0.sql

DOCS        = README.pg_sphere COPYRIGHT.pg_sphere
REGRESS     = init tables points euler circle line ellipse poly path box index \
			  contains_ops contains_ops_compat bounding_box_gist gnomo healpix \
			  moc

REGRESS_9_5 = index_9.5 # experimental for spoint3

TESTS       = init_test tables points euler circle line ellipse poly path box index \
			  contains_ops contains_ops_compat bounding_box_gist gnomo healpix \
			  moc

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
   pgs_gist.sql gnomo.sql \
   healpix.sql pgs_gist_spoint3.sql pgs_moc_type.sql pgs_moc_compat.sql pgs_moc_ops.sql
PGS_SQL_9_5 = pgs_9.5.sql # experimental for spoint3

USE_PGXS = 1
ifdef USE_PGXS
  ifndef PG_CONFIG
    PG_CONFIG := pg_config
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

override CPPFLAGS += -I/usr/include/healpix_cxx
SHLIB_LINK += -lhealpix_cxx
# link a second time as PGXS does not allow to change the linker
PGS_LINKER = g++ $(CXXFLAGS) $(filter-out $(CC) $(CFLAGS), $(LINK.shared))
pgs_link:  $(shlib) $(OBJS) | $(SHLIB_PREREQS)
	$(PGS_LINKER) -o $(shlib) $(OBJS) $(LDFLAGS) $(LDFLAGS_SL) $(SHLIB_LINK)

healpix_bare/healpix_bare.o : healpix_bare/healpix_bare.c
	$(COMPILE.c) -Wno-error=declaration-after-statement -o $@ $^

# experimental for spoint3
pg_version := $(word 2,$(shell $(PG_CONFIG) --version))
pg_version_9_5_plus = $(if $(filter-out 9.1% 9.2% 9.3% 9.4%,$(pg_version)),y,n)
#

## the use of spoint 3 is too experimental and preliminary:
#ifeq ($(pg_version_9_5_plus),y)
#	REGRESS += $(REGRESS_9_5)
#	TESTS   += $(REGRESS_9_5)
#	PGS_SQL += $(PGS_SQL_9_5)
#endif

crushtest: REGRESS += $(CRUSH_TESTS)
crushtest: installcheck


ifeq ($(pg_version_9_5_plus),y)
        PGS_TMP_DIR = --temp-instance=tmp_check
else
        PGS_TMP_DIR = --temp-install=tmp_check --top-builddir=test_top_build_dir
endif

test: pg_sphere.test.sql sql/init_test.sql
	$(pg_regress_installcheck) $(PGS_TMP_DIR) $(REGRESS_OPTS) $(TESTS)

pg_sphere.test.sql: $(RELEASE_SQL) $(shlib)
	tail -n+3 $< | sed 's,MODULE_PATHNAME,$(realpath $(shlib)),g' >$@


$(RELEASE_SQL): $(addsuffix .in, $(RELEASE_SQL) $(PGS_SQL)) pgs_link
	cat $(filter-out pgs_link, $^) > $@

# for "create extension from unpacked*":

UPGRADE_UNP_COMMON =  pgs_types.sql pgs_point.sql pgs_euler.sql pgs_circle.sql \
	pgs_line.sql pgs_ellipse.sql pgs_polygon.sql pgs_path.sql \
	pgs_box.sql pgs_contains_ops_compat.sql pgs_gist.sql \
	pgs_gist_contains_ops.sql contains-ops-fixes-1.sql

AUGMENT_UNP_COMMON = upgrade_scripts/pgs_pre111.sql pgs_contains_ops.sql \
	gnomo.sql
# for vanilla 1.1.1 users
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

# local stuff follows here, next will be "beta2"

AUGMENT_GAVO_111 = $(AUGMENT_UNP_111) healpix.sql # for vanilla 1.1.1 users
UPGRADE_GAVO_111 = $(UPGRADE_UNP_COMMON)

# add new Healpix functions and experimental spoint3
AUGMENT_FROM_GAVO = healpix.sql pgs_gist_spoint3.sql

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

pg_sphere--1.1.5beta0gavo--1.1.5beta2gavo.sql: pgs_moc_type.sql.in
	cat upgrade_scripts/$@.in $^ > $@

pg_sphere--1.1.5beta2gavo--1.1.5beta4gavo.sql: pgs_moc_compat.sql.in
	cat upgrade_scripts/$@.in $^ > $@

pg_sphere--1.1.5beta4gavo--1.2.0.sql: pgs_moc_ops.sql.in
	cat $^ > $@

# end of local stuff

sscan.o : sparse.c

sparse.c: sparse.y
ifdef YACC
	$(YACC) -d $(YFLAGS) -p sphere_yy -o sparse.c $<
else
	@$(missing) bison $< $@
endif

sscan.c : sscan.l
ifdef FLEX
	$(FLEX) $(FLEXFLAGS) -Psphere -o$@ $<
else
	@$(missing) flex $< $@
endif

dist : clean sparse.c sscan.c
	find . -name '*~' -type f -exec rm {} \;
	cd .. && tar --transform s/$(SRC_DIR)/pgsphere-$(PGSPHERE_VERSION)/ --exclude CVS --exclude .git -czf pgsphere-$(PGSPHERE_VERSION).tar.gz $(SRC_DIR) && cd -
