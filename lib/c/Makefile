##
## brickOS - the independent LEGO Mindstorms OS
## Makefile - allows you to keep the upper hand
## (c) 1998 by Markus L. Noga <markus@noga.de>    
##

# our target
LIBTARGET = c
LIBRARY   = lib$(LIBTARGET).a

# sources & objects
#
# Cygwin B-20 can't compile printf.c
ifneq (,$(findstring $(OSTYPE),cygwin32))
	SOURCES = memcpy.c memset.c strcmp.c strcpy.c strlen.c random.c
else
	SOURCES = memcpy.c memset.c strcmp.c strcpy.c strlen.c random.c printf.c
endif

OBJECTS = $(SOURCES:.c=.o)


##
## no user servicable parts below
##

all: promote-stamp
	@#  nothing more to be done here

promote-stamp: $(LIBRARY)
	cp -f $? ../
	@touch $@

include ../../Makefile.common

$(LIBRARY): $(OBJECTS)
	rm -f $@
	$(AR) -sq $@ $(OBJECTS)

.depend: $(SOURCES)
	makedepend -f - $(SOURCES) >.depend

depend: .depend
	@#  nothing more to be done here

clean:
	rm -f *.o *.map *.coff *.srec *.dis* *~ *.bak *.tgz *.s tags *.a

realclean: clean
	rm -f $(LIBRARY) ../$(LIBRARY) .depend
	@rm -f install-stamp promote-stamp

install: install-stamp
	@#  nothing more to be done here

install-stamp: $(LIBRARY)
	@if [ ! -d ${pkglibdir} ]; then \
		mkdir -p ${pkglibdir}; \
	fi
	cp -f $? ${pkglibdir}/$?
	@touch $@

# DO NOT DELETE

