##
## brickOS - the independent LEGO Mindstorms OS
## Makefile - allows you to keep the upper hand
## (c) 1998 by Markus L. Noga <markus@noga.de>    
##

# our target
LIBTARGET = c++
LIBRARY   = lib$(LIBTARGET).a

# sources & objects
SOURCES = stub.c

OBJECTS = $(SOURCES:.c=.o)


##
## no user servicable parts below
##

all: promote-stamp
	@# nothing more to be done here

promote-stamp: $(LIBRARY)
	cp -f $? ../
	@touch $@

include ../../Makefile.common
	
$(LIBRARY): $(OBJECTS)
	rm -f $@
	$(AR) -sq $@ $(OBJECTS)

.depend:
	makedepend -f - $(SOURCES) >.depend

depend: .depend
	@# nothing more to be done here

clean:
	rm -rf *.o *.map *.coff *.srec *.dis* *~ *.bak *.tgz *.s tags 

realclean: clean
	rm -f $(LIBRARY) ../$(LIBRARY) .depend
	@rm -f install-stamp promote-stamp

install: install-stamp
	@# nothing more to be done here

install-stamp: $(LIBRARY)
	@if [ ! -d ${pkglibdir} ]; then \
		mkdir -p ${pkglibdir}; \
	fi
	cp -f $? ${pkglibdir}/$?
	@touch $@

# DO NOT DELETE

