### ==========================================================================
###  $Id: Makefile,v 1.8 2003/01/11 12:31:15 emanlove Exp $
###  FILE: lib/mint/Makefile - make the integer math library
###  brickOS - the independent LEGO Mindstorms OS
### --------------------------------------------------------------------------

# our target
LIBTARGET = mint

# sources 
SOURCES= cmpsi2.c   divhi3.c   modhi3.c   mulhi3.c   udivhi3.c  umodhi3.c \
	 divsi3.c   modsi3.c   mulsi3.c   ucmpsi2.c   udivsi3.c  umodsi3.c


##
## no user servicable parts below
##

LIBRARY   = lib$(LIBTARGET).a

OBJECTS=$(SOURCES:.c=.o)


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
###                    End of FILE: lib/mint/Makefile
### ==========================================================================
