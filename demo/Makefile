##
## legOS - the independent LEGO Mindstorms OS
## Makefile - allows you to keep the upper hand
## (c) 1998, 1999 by Markus L. Noga <markus@noga.de>    
##

# name for the legOS kernel
KERNEL=../boot/legOS

PROGRAMS=helloworld.lx rover.lx linetrack.lx robots.lx c++.lx
# extra dynamic sources
DOBJECTS=

.PHONY: all c++ html depend tag clean realclean

all: $(PROGRAMS) c++

include ../Makefile.common
include ../Makefile.user

# doc/html subdirectory

c++:
	$(MAKE) -C c++ all

html:
	doxygen Doxyfile

depend:
	$(MAKEDEPEND) *.c > .depend

.depend:
	$(MAKEDEPEND) *.c > .depend

tag:
	ctags --format=1 *.c include/*.h include/*/*.h *.c

clean:
	rm -rf .depend* *.o *.dis *.dis2 *~ *.bak *.tgz *.s tags *.ds1 *.ds2 *.dmap *.dcoff *.srec *.map *.coff
	$(MAKE) -C c++ $@

realclean: clean
	rm -f *.lx
	$(MAKE) -C c++ $@

# depencencies
#
ifndef NODEPS
include .depend
endif
