### ==========================================================================
###  $Id: Makefile,v 1.19 2002/10/24 03:18:16 stephmo Exp $
###  FILE: Makefile - make all parts of the brickOS distribution
###  brickOS - the independent LEGO Mindstorms OS
### --------------------------------------------------------------------------
###   (This is the top-level Makefile.  All actions are performed from here)

#  distribution name (all lower-case by convention)
PACKAGE = brickos

#  version of this release, let's use the verision from our VERSION file
VERSION = $(shell cat VERSION)

#  set this once to accommodate our new dir name
export BRICKOS_ROOT=$(shell cd . && pwd)/


# ------------------------------------------------------------
#  No user customization below here...
# ------------------------------------------------------------

#
#  makefile to build the brickOS operating system and demo files
# 
SUBDIRS=util lib boot demo doc

#  if new configuration tool installed force it to be used once
all::
	@if [ configure -nt .configured.flg ]; then \
		rm -f .configured.flg; \
	fi

all install::
	@for i in $(SUBDIRS) ; do $(MAKE) $(MFLAGS) -C $$i $@ || exit 2 ; done

depend tag clean realclean uninstall::
	@for i in $(SUBDIRS) ; do $(MAKE) $(MFLAGS) NODEPS=yes -C $$i $@ || exit 2 ; done

realclean:: clean
	rm -f tags TAGS *.bak

docs-build docs-install::
	$(MAKE) $(MFLAGS) -C doc $@


#  API generation support using Doxygen
#
#  when we get a new version of doxygen, run this target once
#
upgrade-doxygen:
	doxygen -u Doxyfile-c 
	doxygen -u Doxyfile-c++
	doxygen -u Doxyfile

#
#  doc/html-c subdirectory: make C docs
#

pkghtmldir = /usr/local/share/doc/brickos/html

html-c: Doxyfile-c-report

realclean::
	rm -rf doc/html-c doc/rtf-c
	rm -f Doxyfile-c.log Doxyfile-c.rpt .Doxyfile-c-doneflag *.out

brickos-html-c-dist.tar.gz: html-c 
	cd doc;tar --gzip -cf $@ html-c;mv $@ ..;cd -

html-c-dist: brickos-html-c-dist.tar.gz

html-c-install: html-c
	cp -r doc/html-c ${pkghtmldir}

Doxyfile-c.log: Doxyfile-c
	doxygen $? >$@ 2>&1

Doxyfile-c.rpt: Doxyfile-c.log
	@grep Warn $? | sed -e 's/^.*brickos\///' | cut -f1 -d: | sort | \
	 uniq -c | sort -rn | tee $@

.Doxyfile-c-doneflag:  Doxyfile-c.rpt
	@for FIL in `cat $? | cut -c9-99`; do \
	  OUTFILE=`echo $$FIL | sed -e 's/[\/\.]/-/g'`.out; \
	  echo "# FILE: $$OUTFILE" >$$OUTFILE; \
	  grep "$$FIL" $? >>$$OUTFILE; \
	  grep "$$FIL" Doxyfile-c.log | grep Warn >>$$OUTFILE; \
	done
	@touch $@

Doxyfile-c-report: .Doxyfile-c-doneflag
	-ls -ltr *.out 2>/dev/null

#
#  doc/html-c++ subdirectory: make C++ docs
#
html-c++: Doxyfile-c++-report

realclean::
	rm -rf doc/html-c++ doc/rtf-c++
	rm -f Doxyfile-c++.log Doxyfile-c++.rpt .Doxyfile-c++-doneflag *.out

brickos-html-c++-dist.tar.gz: html-c++
	cd doc;tar --gzip -cf $@ html-c++;mv $@ ..;cd -

html-c++-dist: brickos-html-c++-dist.tar.gz

html-c++-install: html-c++
	cp -r doc/html-c++ ${pkghtmldir}

Doxyfile-c++.log: 
	doxygen  Doxyfile-c++ >$@ 2>&1

Doxyfile-c++.rpt: Doxyfile-c++.log
	@grep Warn $? | sed -e 's/^.*brickos\///' | cut -f1 -d: | sort | \
	uniq -c | sort -rn | tee $@

.Doxyfile-c++-doneflag:  Doxyfile-c++.rpt
	@for FIL in `cat Doxyfile-c++.rpt | cut -c9-99`; do \
       OUTFILE=`echo $$FIL | sed -e 's/[\/\.]/-/g'`.out; \
       echo "# FILE: $$OUTFILE" >$$OUTFILE; \
       grep "$$FIL" Doxyfile-c++.rpt >>$$OUTFILE; \
       grep "$$FIL" Doxyfile-c++.log | grep Warn >>$$OUTFILE; \
    done
	@touch $@

Doxyfile-c++-report: .Doxyfile-c++-doneflag
	-ls -ltr *.out 2>/dev/null

#
#  doc/html-kern subdirectory: make kernel developer docs
#
html-kern: Doxyfile-kern-report

realclean::
	rm -rf doc/html-kern doc/rtf-kern
	rm -f Doxyfile-kern.log Doxyfile-kern.rpt .Doxyfile-kern-doneflag *.out

brickos-html-kern-dist.tar.gz: html-kern
	cd doc;tar --gzip -cf $@ html-kern;mv $@ ..;cd -

html-kern-dist: brickos-html-kern-dist.tar.gz

html-kern-install: html-kern
	cp -r doc/html-kern ${pkghtmldir}

Doxyfile-kern.log: 
	doxygen  Doxyfile >$@ 2>&1

Doxyfile-kern.rpt: Doxyfile-kern.log
	@grep Warn $? | sed -e 's/^.*brickos\///' | cut -f1 -d: | sort | \
	uniq -c | sort -rn | tee $@

.Doxyfile-kern-doneflag:  Doxyfile-kern.rpt
	@for FIL in `cat Doxyfile-kern.rpt | cut -c9-99`; do \
       OUTFILE=`echo $$FIL | sed -e 's/[\/\.]/-/g'`.out; \
       echo "# FILE: $$OUTFILE" >$$OUTFILE; \
       grep "$$FIL" Doxyfile-kern.rpt >>$$OUTFILE; \
       grep "$$FIL" Doxyfile-kern.log | grep Warn >>$$OUTFILE; \
    done
	@touch $@

Doxyfile-kern-report: .Doxyfile-kern-doneflag
	-ls -ltr *.out 2>/dev/null

#
#  make all API documentation
#
api-docs-build: html-c html-c++ html-kern

docs: docs-build api-docs-build

#
#  make distribution files for all API documentation
#
api-dist: html-c-dist html-c++-dist html-kern-dist

#
#  install all API documentation
#
docs-install:: html-c-install html-c++-install html-kern-install

#  NOTE: --format=1 is not supported on Linux ([ce]tags in emacs2[01] packages)
#   please set in your own environment
tag::
	-ctags kernel/*.c include/*.h include/*/*.h
	-etags kernel/*.c include/*.h include/*/*.h


.PHONY: all depend tag clean realclean html tag c++ html html-c html-c++ html-kern html-dist 

# ------------------------------------------------------------
#  Components of this release to be packaged
# ------------------------------------------------------------
#
SOURCES = boot demo kernel lib util
HEADERS = include 

EXTRA_DIST = Doxy* doc h8300.rcx configure

DIST_COMMON =  README ChangeLog CONTRIBUTORS LICENSE VERSION Makefile* NEWS TODO README.use


#
# The following 'dist' target code is taken from a makefile 
#   generated by the auto* tools
#
SHELL = /bin/sh

DISTFILES = $(DIST_COMMON) $(SOURCES) $(HEADERS) $(TEXINFOS) $(EXTRA_DIST)

#  locations for this package build effort
SRC_ROOTDIR = .
DISTDIR = $(PACKAGE)-$(VERSION)

#  tools we use to make distribution image
TAR = tar
GZIP_ENV = --best

#  make our distribution tarball
dist: notinsourcetree distdir
	-chmod -R a+r $(DISTDIR)
	GZIP=$(GZIP_ENV) $(TAR) chozf $(DISTDIR).tar.gz $(DISTDIR)
	-rm -rf $(DISTDIR)
	@echo "---------------------------------------------------------"
	@echo "     MD5 sum for $(PACKAGE) v$(VERSION)"
	@md5sum $(DISTDIR).tar.gz
	@echo "---------------------------------------------------------"

#  check for target being run in CVS source tree... bail if is...
notinsourcetree:
	@if [ -d $(SRC_ROOTDIR)/CVS ]; then \
		echo ""; \
		echo "---------------------------------------------------------"; \
		echo "  make: target 'dist' not for use in source tree"; \
		echo "---------------------------------------------------------"; \
		echo ""; \
		exit 2; \
	fi

#  build a copy of the source tree which can be zipped up and then deleted
distdir: $(DISTFILES)
	-rm -rf $(DISTDIR)
	mkdir $(DISTDIR)
	-chmod 777 $(DISTDIR)
#	DISTDIR=`cd $(DISTDIR) && pwd`; 
	@for file in $(DISTFILES); do \
	  d=$(SRC_ROOTDIR); \
	  if test -d $$d/$$file; then \
	    cp -pr $$d/$$file $(DISTDIR)/$$file; \
	  else \
	    test -f $(DISTDIR)/$$file \
	    || ln $$d/$$file $(DISTDIR)/$$file 2> /dev/null \
	    || cp -p $$d/$$file $(DISTDIR)/$$file || :; \
	  fi; \
	done
#  cleanup files which should not make it into any release
	@find $(DISTDIR) -type d -depth -name 'CVS' -exec rm -rf {} \; 
	@find $(DISTDIR) -type f -name '.cvs*' -exec rm -f {} \; 
	@find $(DISTDIR) -type f -name '.dep*' -exec rm -f {} \; 

include Makefile.common		# for the ocnfiguration check

### --------------------------------------------------------------------------
###                      End of top-level brickOS Makefile
### ==========================================================================
