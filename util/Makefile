#
# 2000.03.07 - Paolo Masetti <paolo.masetti@itlug.org>
#
#	- Conditional make based on $OS variable for Windows NT
#	- Replaced firmdl3.c with firmdl dir (easier to port)
#	- Fixed genlds & fixdeps dependencies
#
# 2000.06.17 - Paolo Masetti <paolo.masetti@itlug.org>
#
#	- Removed firmdl3.c compile for *nix
#

#
# Winnt executables are in the form "*.exe"
#

include Makefile.common

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
	@#  nothing more to do here...

all clean depend install realclean::
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

realclean:: clean
	rm -f $(TARGETS)
	@rm -f install-stamp

