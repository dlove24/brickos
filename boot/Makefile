##
## legOS - the independent LEGO Mindstorms OS
## Makefile - allows you to keep the upper hand
## (c) 1998, 1999 by Markus L. Noga <markus@noga.de>    
##

## Modification to allow perl-less compilation by Luis Villa 07/17/00

# legOS kernel used (w/o extension)
KERNEL=legOS

# uncomment out the first line if you need a kernel disassembly file.
# This will not work on Windows unless you have perl installed.
#all: $(KERNEL).srec $(KERNEL).dis2 $(KERNEL).lds
all: $(KERNEL).srec $(KERNEL).lds

include ../Makefile.common
include ../Makefile.kernel

# doc/html subdirectory
html:
	doxygen Doxyfile

.depend:
	$(MAKEDEPEND) $(LEGOS_ROOT)kernel/*.c > .depend

depend:
	$(MAKEDEPEND) $(LEGOS_ROOT)kernel/*.c > .depend

tag:
	ctags --format=1 *.c include/*.h include/*/*.h *.c

clean:

realclean:
	rm -rf .depend* *.o *.dis *~ *.bak *.tgz *.s tags *.dcoff *.dmap *.dsrec *.dis2
	rm -f  *.srec *.map *.coff *.lds 

# depencencies
#
ifndef NODEPS
include .depend
endif
