##
## legOS - the independent LEGO Mindstorms OS
## Makefile - allows you to keep the upper hand
## (c) 1998 by Markus L. Noga <markus@noga.de>    
##

# our target
LIBTARGET=c

# sources & objects
#
# Cygwin B-20 can't compile printf.c
ifneq (,$(findstring $(OSTYPE),cygwin32))
	SOURCES= memcpy.c memset.c strcmp.c strcpy.c strlen.c random.c
else
	SOURCES= memcpy.c memset.c strcmp.c strcpy.c strlen.c random.c printf.c
endif

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
