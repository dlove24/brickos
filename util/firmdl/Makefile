### ==========================================================================
###  $Id: Makefile,v 1.8 2002/10/23 17:42:47 stephmo Exp $
###  FILE: util/firmdl/Makefile - make firmware download utility
###  brickOS - the independent LEGO Mindstorms OS
### --------------------------------------------------------------------------

# specify environment before including the common stuff
BUILDING_HOST_UTILS = true
include ../../Makefile.common

FIRMDL3 = firmdl3$(EXT)
MKIMG = mkimg$(EXT)
MAN1 = firmdl3.1

ALL_TARGETS = ../$(FIRMDL3) $(MKIMG) 


all:: fastdl.h $(ALL_TARGETS)
	@# nothing to do here but do it silently

$(MKIMG): mkimg.o srec.o
	$(CC) $^ -o $@ $(CFLAGS)

../$(FIRMDL3): firmdl.o srec.o rcx_comm.o
	$(CC) $^ -o $@ $(CFLAGS)

fastdl.h: $(MKIMG) fastdl.srec
	./$(MKIMG) fastdl.srec > $@

depend::
	@# nothing to do here but do it silently

install: install-stamp 
	@# nothing to do here but do it silently

install-stamp: $(ALL_TARGETS) $(MAN1)
	cp -f ../$(FIRMDL3) $(bindir)
	@if [ ! -d ${mandir}/man1 ]; then \
		mkdir -p ${mandir}/man1; \
	fi
	cp -f $(MAN1) $(mandir)/man1/$(MAN1)
	@touch $@

uninstall:
	rm -f $(mandir)/man1/$(MAN1) $(bindir)/$(FIRMDL3) install-stamp

clean:
	rm -f *.o *~ *.bak

realclean: clean
	rm -f fastdl.h $(ALL_TARGETS)
	@rm -f install-stamp

.PHONY: realclean clean install depend all


### --------------------------------------------------------------------------
###                   End of FILE: util/firmdl/Makefile
### ==========================================================================
