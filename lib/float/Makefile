### ==========================================================================
###  $Id: Makefile,v 1.9 2003/01/11 12:30:53 emanlove Exp $
###  FILE: lib/float/Makefile - make the floating point math library
###  brickOS - the independent LEGO Mindstorms OS
### --------------------------------------------------------------------------

# our target
LIBTARGET = float

# sources 
SOURCES = expandsf.s joinsf.s addsf3.s negsf2.s mulsf3.s divsf3.s \
          floatsisf.s cmpsf2.s fixsfsi.s startsf.s normalsf.s


##
## no user servicable parts below
##

LIBRARY   = lib$(LIBTARGET).a

OBJECTS = $(SOURCES:.s=.o)


all:: promote-stamp
	@# nothing to do here but do it silently

promote-stamp: $(LIBRARY)
	cp -f $? ../
	@touch $@

include ../../Makefile.common

$(LIBRARY): $(OBJECTS)
	rm -f $@
	$(AR) -sq $@ $(OBJECTS)

.depend: $(SOURCES)
	$(MAKEDEPEND) $(SOURCES) > .depend

depend:: .depend
	@# nothing to do here but do it silently

clean:
	rm -f *.o *.map *.coff *.srec *.dis* *~ *.bak *.tgz 

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
###                    End of FILE: lib/float/Makefile
### ==========================================================================
