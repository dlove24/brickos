### ==========================================================================
###  $Id: Makefile,v 1.7 2002/10/15 06:53:55 stephmo Exp $
###  FILE: boot/Makefile - make the brickOS kernel
###  brickOS - the independent LEGO Mindstorms OS
### --------------------------------------------------------------------------

# brickOS kernel used (w/o extension)
KERNEL=brickOS

# uncomment out the first line if you need a kernel disassembly file.
# This will not work on Windows unless you have perl installed.
#
#all: $(KERNEL).srec $(KERNEL).dis2 $(KERNEL).lds
all: $(KERNEL).srec $(KERNEL).lds

include ../Makefile.common
include ../Makefile.kernel

.depend: $(BRICKOS_ROOT)kernel/*.c
	$(MAKEDEPEND) $(BRICKOS_ROOT)kernel/*.c > .depend

depend: .depend

install:
	@# nothing to do here but do it silently

tag:
	@# nothing to do here but do it silently

clean:
	rm -f *.srec *.o *.lds *.dis *.coff *.map *.dis2

realclean:
	rm -rf .depend* *.o *.dis *~ *.bak *.tgz *.s tags *.dcoff *.dmap *.dsrec *.dis2
	rm -f  *.srec *.map *.coff *.lds

.PHONY: all tag clean realclean depend

# depencencies
#
ifndef NODEPS
include .depend
endif

### --------------------------------------------------------------------------
###                         End of FILE: boot/Makefile
### ==========================================================================
