### ==========================================================================
###  $Id: Makefile,v 1.8 2003/01/11 12:30:08 emanlove Exp $
###  FILE: lib/c/Makefile - make the c suport library
###  brickOS - the independent LEGO Mindstorms OS
### --------------------------------------------------------------------------

# our target
LIBTARGET = c

# sources 
#
# Cygwin B-20 can't compile printf.c   FIXME  This is busted in new evironment
ifneq (,$(findstring $(OSTYPE),cygwin32))
	SOURCES = memcpy.c memset.c strcmp.c strcpy.c strlen.c random.c
else
	SOURCES = memcpy.c memset.c strcmp.c strcpy.c strlen.c random.c printf.c
endif


## 
## no user servicable parts below
##

LIBRARY   = lib$(LIBTARGET).a

OBJECTS = $(SOURCES:.c=.o)


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
###                        End of FILE: lib/c/Makefile
### ==========================================================================
