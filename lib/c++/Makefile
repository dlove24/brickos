### ==========================================================================
###  $Id: Makefile,v 1.9 2004/01/25 21:59:42 hzeller Exp $
###  FILE: lib/c++/Makefile - make the c++ library
###  brickOS - the independent LEGO Mindstorms OS
### --------------------------------------------------------------------------

# our target
LIBTARGET = c++

# sources 
CSOURCES = stub.c
CXXSOURCES=mem.cpp

##
## no user servicable parts below
##

LIBRARY   = lib$(LIBTARGET).a

OBJECTS = $(CSOURCES:.c=.o) $(CXXSOURCES:.cpp=.o)


all:: promote-stamp
	@# nothing to do here but do it silently

promote-stamp: $(LIBRARY)
	cp -f $? ../
	@touch $@

include ../../Makefile.common

$(LIBRARY): $(OBJECTS)
	rm -f $@
	$(AR) -sq $@ $(OBJECTS)

.depend: $(CSOURCES) $(CXXSOURCES)
	$(MAKEDEPEND) $(CSOURCES) $(CXXSOURCES) > .depend

depend:: .depend
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

uninstall:
	@rm -f install-stamp

.PHONY: all promote-stamp depend clean realclean install install-stamp uninstall

### --------------------------------------------------------------------------
###                        End of FILE: lib/c++/Makefile
### ==========================================================================
