##
## brickOS - the independent LEGO Mindstorms OS
## Makefile - allows you to keep the upper hand
## (c) 1998 by Markus L. Noga <markus@noga.de>
##

# our target
LIBTARGET=mint

# sources & objects
SOURCES= cmpsi2.c   divhi3.c   modhi3.c   mulhi3.c   udivhi3.c  umodhi3.c \
	 divsi3.c   modsi3.c   mulsi3.c   ucmpsi2.c   udivsi3.c  umodsi3.c

OBJECTS=$(SOURCES:.c=.o)


##
## no user servicable parts below
##

all: lib$(LIBTARGET).a $(OBJECTS)
	cp -f lib$(LIBTARGET).a ../

include ../../Makefile.common

lib$(LIBTARGET).a: $(OBJECTS)
	rm -f lib$(LIBTARGET).a
	$(AR) -sq lib$(LIBTARGET).a $(OBJECTS)

depend:
	makedepend $(SOURCES)

clean:
	rm -rf *.o *.map *.coff *.srec *.dis* *~ *.bak *.tgz *.s tags *.a
