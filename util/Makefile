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
EXECUTABLES= fontdesign dll makelx fixdeps genlds firmdl3

# Needed for DOS/WIN32 platforms
ifdef EXT
	TARGETS=$(addsuffix $(EXT),$(EXECUTABLES))
else
	TARGETS=$(EXECUTABLES)
endif

all: $(TARGETS)

# take off debug symbols
strip:
	strip $(TARGETS)

clean:
	$(MAKE) $(MFLAGS) -C dll-src clean
	$(MAKE) $(MFLAGS) -C firmdl clean

makelx$(EXT) dll$(EXT) genlds$(EXT) fixdeps$(EXT):
	$(MAKE) $(MFLAGS) -C dll-src || exit 2

firmdl3$(EXT):
	$(MAKE) $(MFLAGS) -C firmdl || exit 2

fontdesign$(EXT):	fontdesign.c
	$(CC) fontdesign.c -o $@ $(CFLAGS)

depend:
	$(MAKE) $(MFLAGS) -C dll-src depend
	$(MAKE) $(MFLAGS) -C firmdl depend


realclean: clean
	rm -f $(TARGETS)
