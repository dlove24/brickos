##
## brickOS - the independent LEGO Mindstorms OS
## Makefile - allows you to keep the upper hand
## (c) 1998 by Markus L. Noga <markus@noga.de>
##

SUBDIRS=c      mint      float   c++

all clean realclean depend::
	@for i in $(SUBDIRS); do $(MAKE) $(MFLAGS) -C $$i $@ || exit 2; done

install:
	@#  nothing to do here..., yet
