### ==========================================================================
###  $Id: Makefile,v 1.4 2002/10/15 03:23:24 stephmo Exp $
###  FILE: lib/c++/Makefile - make the c++ library
###  brickOS - the independent LEGO Mindstorms OS
### --------------------------------------------------------------------------

# our target
LIBTARGET = c++

# sources 
SOURCES = stub.c


##
## no user servicable parts below
##

LIBRARY   = lib$(LIBTARGET).a

OBJECTS = $(SOURCES:.c=.o)


all: promote-stamp
	@# nothing to do here but do it silently

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
	@# nothing to do here but do it silently

clean:
	rm -f *.o *.map *.coff *.srec *.dis* *~ *.bak *.tgz *.s

realclean: clean
	rm -f $(LIBRARY) ../$(LIBRARY) .depend
	@rm -f install-stamp promote-stamp tags TAGS

install: install-stamp
	@# nothing to do here but do it silently

install-stamp: $(LIBRARY)
	@if [ ! -d ${pkglibdir} ]; then \
		mkdir -p ${pkglibdir}; \
	fi
	cp -f $? ${pkglibdir}/$?
	@touch $@

### --------------------------------------------------------------------------
###                        End of FILE: lib/c++/Makefile
### ==========================================================================
