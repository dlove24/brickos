##
## brickOS - the independent LEGO Mindstorms OS
## Makefile - allows you to keep the upper hand
## (c) 1998 by Markus L. Noga <markus@noga.de>
##

# our target
LIBTARGET = mint
LIBRARY   = lib$(LIBTARGET).a

# sources & objects
SOURCES= cmpsi2.c   divhi3.c   modhi3.c   mulhi3.c   udivhi3.c  umodhi3.c \
	 divsi3.c   modsi3.c   mulsi3.c   ucmpsi2.c   udivsi3.c  umodsi3.c

OBJECTS=$(SOURCES:.c=.o)


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

