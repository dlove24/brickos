### ==========================================================================
###  $Id: Makefile,v 1.6 2002/10/15 22:03:45 stephmo Exp $
###  FILE: util/Makefile - make the hosted utilities
###  brickOS - the independent LEGO Mindstorms OS
### --------------------------------------------------------------------------

# specify environment before including the common stuff
BUILDING_HOST_UTILS = true
include ../Makefile.common

# Define here the executable files to be build
EXECUTABLES    = fontdesign 
UTILITY_SCRIPT = merge-map

# Needed for DOS/WIN32 platforms
ifdef EXT
	TARGETS = $(addsuffix $(EXT),$(EXECUTABLES))
else
	TARGETS = $(EXECUTABLES)
endif

SUBDIRS = dll-src firmdl

all:: $(TARGETS)
	@# nothing to do here but do it silently

all clean depend install realclean uninstall::
	@for i in $(SUBDIRS) ; do $(MAKE) $(MFLAGS) NODEPS=yes -C $$i $@ || exit 2 ; done

# remove debug symbols
strip:
	strip $(TARGETS)

# build our one local program
fontdesign$(EXT):	fontdesign.c

install:: install-stamp

install-stamp: $(TARGETS) $(UTILITY_SCRIPT)
	@if [ ! -d ${pkglibdir} ]; then \
		mkdir -p ${pkglibdir}; \
	fi
	cp -f $(TARGETS) ${pkglibdir}
	cp -f $(UTILITY_SCRIPT) ${pkglibdir}
	@touch $@

tag::
	@# nothing to do here but do it silently

realclean:: clean
	rm -f $(TARGETS)
	@rm -f install-stamp


### --------------------------------------------------------------------------
###                         End of FILE: util/Makefile
### ==========================================================================
