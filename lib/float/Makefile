##
## brickOS - the independent LEGO Mindstorms OS
## Makefile - allows you to keep the upper hand
## (c) 1998 by Markus L. Noga <markus@noga.de>    
##

# our target
LIBTARGET = float
LIBRARY   = lib$(LIBTARGET).a

# sources & objects
SOURCES = expandsf.s joinsf.s addsf3.s negsf2.s mulsf3.s divsf3.s \
          floatsisf.s cmpsf2.s fixsfsi.s startsf.s normalsf.s
OBJECTS = $(SOURCES:.s=.o)


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
	rm -f *.o *.map *.coff *.srec *.dis* *~ *.bak *.tgz tags 

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

