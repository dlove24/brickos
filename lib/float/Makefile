##
## brickOS - the independent LEGO Mindstorms OS
## Makefile - allows you to keep the upper hand
## (c) 1998 by Markus L. Noga <markus@noga.de>    
##

# our target
LIBTARGET=float

# sources & objects
SOURCES = expandsf.s joinsf.s addsf3.s negsf2.s mulsf3.s divsf3.s \
          floatsisf.s cmpsf2.s fixsfsi.s startsf.s normalsf.s
OBJECTS = $(SOURCES:.s=.o)


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
	

clean:
	rm -rf *.o *.map *.coff *.srec *.dis* *~ *.bak *.tgz tags *.a
