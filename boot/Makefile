### ==========================================================================
###  $Id: Makefile,v 1.16 2003/02/16 20:43:49 markril Exp $
###  FILE: boot/Makefile - make the brickOS kernel
###  brickOS - the independent LEGO Mindstorms OS
### --------------------------------------------------------------------------

# brickOS kernel used (w/o extension)
KERNEL=brickOS

# kernel source files
KSOURCES=kmain.c mm.c systime.c tm.c semaphore.c conio.c lcd.c \
	 lnp-logical.c lnp.c remote.c program.c vis.c battery.c\
         timeout.c dkey.c dmotor.c dsensor.c dsound.c swmux.c\
         critsec.c setjmp.c

KERNEL_TARGETS = $(KERNEL).srec \
                 $(KERNEL).lds

#
# NOTE: uncomment out the first line if you need a kernel disassembly file.
# This will not work on Windows unless you have perl installed.
#
#all: $(KERNEL).srec $(KERNEL).dis2 $(KERNEL).lds
all:: $(KERNEL_TARGETS)

include ../Makefile.common

.depend: $(BRICKOS_ROOT)/kernel/*.c
	$(MAKEDEPEND) $(BRICKOS_ROOT)/kernel/*.c > .depend

depend:: .depend

install: install-stamp
	@# nothing to do here but do it silently

install-stamp: ${pkgincludedir} ${pkgexampledir} ${pkglibdir} \
               ../include/* ../demo/Makefile ../demo/*.c ../demo/c++/*.C \
			   config.h $(KERNEL_TARGETS) ../Makefile.user ../Makefile.common
	cp -r ../include/* ${pkgincludedir}
	@find ${pkgincludedir} -type d -name 'CVS' -depth -exec rm -rf {} \;
	cp -r ../demo ${pkgexampledir}
	@cat ../demo/Makefile | sed -e 's/\.\.\/Makefile/$$\(LIBDIR\)\/Makefile/g' | \
	                       sed -e 's/\.\.\/include/$$\(INCDIR\)/g' | \
	                       sed -e 's:KERNEL\=\.\..boot.brickOS:KERNEL = $$\(LIBDIR\)/brickOS:' | \
	                       sed -e 's:\# \#LIBDIR\#.*:LIBDIR \= ${pkglibdir}:' | \
						   sed -e 's/.BRICKOS_ROOT..util/(LIBDIR)/g' >../demo/Makefile.dist
	@cat ../demo/c++/Makefile | sed -e 's/\.\..\.\..Makefile/$$\(LIBDIR\)\/Makefile/g' | \
	                            sed -e 's/..BRICKOS_ROOT..include/$$\(INCDIR\)/g' | \
	                            sed -e 's:KERNEL \= .*:KERNEL = $$\(LIBDIR\)/brickOS:' | \
	                            sed -e 's:\# \#LIBDIR\#.*:LIBDIR \= ${pkglibdir}:' | \
						        sed -e 's/..ROOTDIR.util/(LIBDIR)/g' >../demo/c++/Makefile.dist
	cp -f ../demo/Makefile.dist ${pkgexampledir}/demo/Makefile
	cp -f ../demo/Makefile.dist ${pkgexampledir}/Makefile
	cp -f ../demo/c++/Makefile.dist ${pkgexampledir}/demo/c++/Makefile
	@find ${pkgexampledir} -type f -name '*.dist' -exec rm -f {} \;
	@find ${pkgexampledir} -type f -name '*.lx' -exec rm -f {} \;
	@find ${pkgexampledir} -type d -name 'CVS' -depth -exec rm -rf {} \;
	@find ${pkgexampledir} -type f -name '*.o' -depth -exec rm -f {} \;
	@find ${pkgexampledir} -type f -name '*.ref' -depth -exec rm -f {} \;
	cp -f $(KERNEL_TARGETS) ${pkglibdir}
	@echo "/* WARNING: INSTALLED config.h: do not change this file without re-building brickOS kernel! */" >config.h.dist
	@cat config.h >>config.h.dist
	cp -f config.h.dist ${pkgincludedir}/config.h
	@cat ../Makefile.common | sed -e 's/.BRICKOS_ROOT..lib/(LIBDIR)/g' | \
	                         sed -e 's/^all:.*//' | \
	                         sed -e 's/.BRICKOS_ROOT..include/(INCDIR)/g' | \
							 sed -e 's/.BRICKOS_ROOT..util/(LIBDIR)/g' >../Makefile.common.dist
	@cat ../Makefile.user | sed -e 's/.BRICKOS_ROOT..lib/(LIBDIR)/g' | \
                             sed -e 's/.BRICKOS_ROOT..include/(INCDIR)/g' | \
                             sed -e 's/.BRICKOS_ROOT..boot/(INCDIR)/g' | \
							 sed -e 's/.BRICKOS_ROOT..util/(LIBDIR)/g' >../Makefile.user.dist
	cp -f ../Makefile.user.dist ${pkglibdir}/Makefile.user
	@echo "" >>../Makefile.common.dist
	@echo "INCDIR = ${pkgincludedir}" >>../Makefile.common.dist
	@echo "BINDIR = ${bindir}" >>../Makefile.common.dist
	@echo "LIBDIR = ${pkglibdir}" >>../Makefile.common.dist
	@echo "" >>../Makefile.common.dist
	@echo "### --------------------------------------------------------------------------" >>../Makefile.common.dist
	@echo "###                         End of FILE: Makefile.common" >>../Makefile.common.dist
	@echo "### ==========================================================================" >>../Makefile.common.dist
	cp -f ../Makefile.common.dist ${pkglibdir}/Makefile.common
	@find ${pkgexampledir} -type f -exec chmod 444 {} \;
	@touch $@

clean::
	rm -f ../Makefile.user.dist ../Makefile.common.dist ../demo/Makefile.dist config.h.dist


#!/bin/bash -p


${pkgincludedir} ${pkgexampledir} ${pkglibdir}:
	mkdir -p $@

uninstall:
	rm -rf ${pkglibdir} ${pkgincludedir} ${pkgdocdir} install-stamp

tag:
	@# nothing to do here but do it silently

clean::
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
LDFLAGS=-T $(BRICKOS_ROOT)/h8300.rcx -relax -L$(BRICKOS_ROOT)/lib

#
# 2002.05.12 - Ted Hess <thess@users.sourceforge.net>
#
#	- Allow setting LNP Host Address on command line
ifdef LNP_HOSTADDR
CDEFINES =-DCONF_LNP_HOSTADDR=$(LNP_HOSTADDR)
endif

############ kernel stuff

# how to build coff kernel (for symbols, disassembly etc)
$(KERNEL).coff: $(KOBJECTS)
	$(LD) $(LDFLAGS) $(KOBJECTS) $(LIBS) -o $@
	chmod a-x $@

# how to build srec kernel (for download)
%.srec: %.coff %.map
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
