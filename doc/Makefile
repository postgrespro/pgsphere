#----------------------------------------------------------------------------
#
# pgSphere documentation generation
#
#----------------------------------------------------------------------------

ifndef PGSPHERE_VERSION
include ../Makefile.common.mk
ifndef PGSPHERE_VERSION
$(error PGSPHERE_VERSION is not set)
endif
endif

ifndef FOP
FOP = fop
endif

ifdef XMLLINT
XMLLINT := $(XMLLINT) --nonet
else
XMLLINT = xmllint --nonet
endif

ifdef XSLTPROC
XSLTPROC := $(XSLTPROC) --nonet
else
XSLTPROC = xsltproc --nonet
endif

override XSLTPROCFLAGS += \
	--path stylesheets --path img --path . \
	--stringparam pg_sphere.version '$(PGSPHERE_VERSION)' \
	--stringparam pg.version '$(PGSPHERE_VERSION)'

XMLINCLUDE = --path .
ALLSGML := $(wildcard *.sgm)
ALLIMAGES := $(wildcard img/*.jpg)

all: html pdf

.PHONY: all html pdf clean

# This line fixes the error like:
# No rule to make target 'pg_sphere.control'
.PHONY: pg_sphere.control

version.xml:
	@echo $(PGSPHERE_VERSION) > version.xml

pg_sphere-full.xml: version.xml
pg_sphere-full.xml: pg_sphere.xml $(ALLSGML)
	$(XMLLINT) $(XMLINCLUDE) --output $@ --noent --valid $<

#------------------------------------------------------------------------------
# HTML
#------------------------------------------------------------------------------

XSLTPROC_HTML_MULTIPAGE_FLAGS := --stringparam img.src.path ''
XSLTPROC_HTML_SINGLEPAGE_FLAGS := --stringparam img.src.path ''

html: html-singlepage html-multipage

html-multipage: stylesheets/stylesheet.xsl pg_sphere-full.xml version.xml $(ALLIMAGES)
	$(XSLTPROC) $(XMLINCLUDE) $(XSLTPROCFLAGS) $(XSLTPROC_HTML_MULTIPAGE_FLAGS) $(wordlist 1,2,$^)
	mkdir -p html/img
	cp $(ALLIMAGES) html/img

html-singlepage: pg_sphere-$(PGSPHERE_VERSION).html

pg_sphere-$(PGSPHERE_VERSION).html: stylesheets/stylesheet-html-nochunk.xsl pg_sphere-full.xml version.xml $(ALLIMAGES)
	$(XSLTPROC) $(XMLINCLUDE) $(XSLTPROCFLAGS) $(XSLTPROC_HTML_SINGLEPAGE_FLAGS) -o $@ $(wordlist 1,2,$^)

#------------------------------------------------------------------------------
# PDF
#------------------------------------------------------------------------------

pdf: pg_sphere-${PGSPHERE_VERSION}-A4.pdf pg_sphere-${PGSPHERE_VERSION}-US.pdf

pg_sphere-$(PGSPHERE_VERSION)-A4.pdf: pg_sphere.A4.fo
	$(FOP) -v -fo $< -pdf $@

pg_sphere-$(PGSPHERE_VERSION)-US.pdf: pg_sphere.US.fo
	$(FOP) -v -fo $< -pdf $@

pg_sphere.A4.fo: stylesheets/stylesheet-fo.xsl pg_sphere-full.xml version.xml $(ALLIMAGES)
	$(XSLTPROC) $(XMLINCLUDE) $(XSLTPROCFLAGS) $(XSLTPROC_FO_FLAGS) --stringparam paper.type A4 -o $@ stylesheets/stylesheet-fo.xsl pg_sphere-full.xml

pg_sphere.US.fo: stylesheets/stylesheet-fo.xsl pg_sphere-full.xml version.xml $(ALLIMAGES)
	$(XSLTPROC) $(XMLINCLUDE) $(XSLTPROCFLAGS) $(XSLTPROC_FO_FLAGS) --stringparam paper.type USLetter -o $@ stylesheets/stylesheet-fo.xsl pg_sphere-full.xml

#------------------------------------------------------------------------------
# Cleanup
#------------------------------------------------------------------------------

clean distclean:
	rm -rf ./html
	rm -f version.xml
	rm -f pg_sphere-full.xml
	rm -f pg_sphere.A4.fo
	rm -f pg_sphere.US.fo
	rm -f pg_sphere-$(PGSPHERE_VERSION).html
	rm -f pg_sphere-$(PGSPHERE_VERSION)-A4.pdf
	rm -f pg_sphere-$(PGSPHERE_VERSION)-US.pdf
