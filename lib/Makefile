##
## brickOS - the independent LEGO Mindstorms OS
## Makefile - allows you to keep the upper hand
## (c) 1998 by Markus L. Noga <markus@noga.de>
##

SUBDIRS=c      mint      float   c++
LIBS=libc.a libmint.a libfloat.a libc++.a

all: $(LIBS)

clean:
	for i in $(SUBDIRS); do $(MAKE) $(MFLAGS) -C $$i clean ; done

lib%.a: %
	$(MAKE) $(MFLAGS) -C $* || exit 2

depend:

realclean: clean
	rm -f $(LIBS)
