### ==========================================================================
###  $Id: Makefile,v 1.7 2002/10/17 05:05:40 stephmo Exp $
###  FILE: demo/Makefile - make the C Language Demo Programs
###  brickOS - the independent LEGO Mindstorms OS
### --------------------------------------------------------------------------

# name for the brickOS kernel
# #LIBDIR#         ###   DO NOT REMOVE THIS LINE  ###
KERNEL=../boot/brickOS

PROGRAMS=helloworld.lx rover.lx linetrack.lx robots.lx c++.lx sound.lx trailerbot.lx

# extra dynamic sources
DOBJECTS=

all:: $(PROGRAMS) 

include ../Makefile.common
include ../Makefile.user

#  for these targets, pass them on to C++ dir, too
all clean realclean depend::
	$(MAKE) -C c++ $@

.depend: *.c
	$(MAKEDEPEND) *.c > .depend

depend:: .depend

install uninstall:
	@# nothing to do here but do it silently

#  NOTE: --format=1 is not supported on Linux ([ce]tags in emacs2[01] packages)
#   please set in your own environment
tag:
	-ctags *.c ../include/*.h ../include/*/*.h 
	-etags *.c ../include/*.h ../include/*/*.h 

clean::
	rm -f *.o *.dis *.dis2 *~ *.bak *.tgz *.s *.ds1 *.ds2 *.dmap *.dcoff 
	rm -f *.srec *.map *.coff

realclean:: clean
	rm -f *.lx .depend tags TAGS

.PHONY: all depend tag clean realclean

# depencencies
#
ifndef NODEPS
include .depend
endif

### --------------------------------------------------------------------------
###                          End of FILE: demo/Makefile
### ==========================================================================
