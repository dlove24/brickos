##
## legOS - the independent LEGO Mindstorms OS
## Makefile - allows you to keep the upper hand
## (c) 1998 by Markus L. Noga <markus@noga.de>    
##

# our target
LIBTARGET=c++

# sources & objects
OBJECTS= stub.o

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
	makedepend *.c *.cpp *.C

clean:
	rm -rf *.o *.map *.coff *.srec *.dis* *~ *.bak *.tgz *.s tags *.a
