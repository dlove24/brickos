### ==========================================================================
###  $Id: Makefile,v 1.9 2002/10/15 22:03:45 stephmo Exp $
###  FILE: boot/Makefile - make the brickOS kernel
###  brickOS - the independent LEGO Mindstorms OS
### --------------------------------------------------------------------------

# brickOS kernel used (w/o extension)
KERNEL=brickOS

# kernel source files
KSOURCES=kmain.c mm.c systime.c tm.c semaphore.c conio.c lcd.c \
	 lnp-logical.c lnp.c remote.c program.c vis.c battery.c\
         timeout.c dkey.c dmotor.c dsensor.c dsound.c swmux.c\
         critsec.c

KERNEL_TARGETS = $(KERNEL).srec \
                 $(KERNEL).lds

#
# NOTE: uncomment out the first line if you need a kernel disassembly file.
# This will not work on Windows unless you have perl installed.
#
#all: $(KERNEL).srec $(KERNEL).dis2 $(KERNEL).lds
all: $(KERNEL_TARGETS)

include ../Makefile.common

.depend: $(BRICKOS_ROOT)kernel/*.c
	$(MAKEDEPEND) $(BRICKOS_ROOT)kernel/*.c > .depend

depend: .depend

install: install-stamp
	@# nothing to do here but do it silently

install-stamp: ${pkgincludedir} ${pkgexampledir} ${pkglibdir} \
               ../include/* ../demo/Makefile ../demo/*.c ../demo/c++/*.C \
			   config.h $(KERNEL_TARGETS) ../Makefile.user ../Makefile.common
	cp -r ../include/* ${pkgincludedir}
	@find ${pkgincludedir} -type d -name 'CVS' -depth -exec rm -rf {} \;
	cp -f ../demo/Makefile ${pkgexampledir}
	cp -r ../demo ${pkgexampledir}
	@find ${pkgexampledir} -type d -name 'CVS' -depth -exec rm -rf {} \;
	@find ${pkgexampledir} -type f -name '*.o' -depth -exec rm -f {} \;
	@find ${pkgexampledir} -type f -name '*.ref' -depth -exec rm -f {} \;
	cp -f $(KERNEL_TARGETS) ${pkglibdir}
	cp -f config.h ${pkglibdir}
	cp -f ../Makefile.user ../Makefile.common ${pkglibdir}
	touch $@

${pkgincludedir} ${pkgexampledir} ${pkglibdir}:
	mkdir -p $@

uninstall:
	rm -rf ${pkglibdir} ${pkgincludedir} ${pkgdocdir} install-stamp

tag:
	@# nothing to do here but do it silently

clean:
	rm -f *.srec *.o *.lds *.dis *.coff *.map *.dis2

realclean:
	rm -rf .depend* *.o *.dis *~ *.bak *.tgz *.s tags *.dcoff *.dmap *.dsrec *.dis2
	rm -f  *.srec *.map *.coff *.lds install-stamp

.PHONY: all tag clean realclean depend

##
## no user servicable parts below
##

# kernel object files
KOBJECTS=$(KSOURCES:.c=.o)

# Kernel libraries. Use second version to link all
# libraries statically to kernel. Change Makefile.user, too.
LIBS=-lc -lmint
#LIBS=--whole-archive -lc -lmint -lfloat

# linker command files for kernel
LDFLAGS=-T $(BRICKOS_ROOT)h8300.rcx -relax -L$(BRICKOS_ROOT)lib

#
# 2002.05.12 - Ted Hess <thess@users.sourceforge.net>
#
#	- Allow setting LNP Host Address on command line
ifdef LNP_HOSTADDR
CDEFINES =-DCONF_LNP_HOSTADDR=$(LNP_HOSTADDR)
endif

############ kernel stuff

# how to build coff kernel (for symbols, disassembly etc)
%.coff: $(KOBJECTS)
	$(LD) $(LDFLAGS) $(KOBJECTS) $(LIBS) -o $*.coff
	chmod a-x $*.coff

# how to build srec kernel (for download)
%.srec: %.coff
	$(OBJCOPY) -I coff-h8300 -O srec $(NEED_ZERO_PADDING) $*.coff $*.srec
	chmod a-x $*.srec

# how to make map files
%.map: %.coff
	$(NM) $*.coff | sort -u > $*.map

# how to build linker script for dynamic executables
%.lds: %.map
	$(GENLDS) $* < $*.map > $*.lds

# how to disassemble coff kernel
%.dis: %.coff
	$(OBJDUMP) $(ODFLAGS) $*.coff > $*.dis

# how to merge map files with symbols
%.dis2: %.map %.dis
	$(MERGEMAP) $*.map $*.dis > $*.dis2

# how to disassemble srec kernel (don't do this.)
%.dis: %.srec
	$(OBJDUMP) $(ODFLAGS) $*.srec > $*.dis


############ original firmware stuff

# how to disassemble original firmware file
%.dis: %.lgo
	$(OBJDUMP) $(ODFLAGS) $*.lgo > $*.dis

# how to merge labels etc into disassembly
%.s: %.fix $(FIRMVERS).dis
	./fixfirm.pl $*.fix $(FIRMVERS).dis > $*.s

# how to reassemble new firmware file
%.srec: %.s
	$(AS) $*.s -o $*.o
	$(LD) -Ttext=0x8000 $*.o -o $*.out
	$(OBJCOPY) -O srec $*.out $*.srec
	rm -f $*.o $*.out
	chmod -x $*.srec

#  dependencies 
ifndef NODEPS
include .depend
endif

### --------------------------------------------------------------------------
###                         End of FILE: boot/Makefile
### ==========================================================================
