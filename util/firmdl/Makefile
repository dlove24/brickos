# Makefile for firmdl3

# The contents of this file are subject to the Mozilla Public License
# Version 1.0 (the "License"); you may not use this file except in
# compliance with the License. You may obtain a copy of the License at
# http://www.mozilla.org/MPL/
#
# Software distributed under the License is distributed on an "AS IS"
# basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
# License for the specific language governing rights and limitations
# under the License.
#
# The Original Code is Firmdl code, released October 3, 1998.
#
# The Initial Developer of the Original Code is Kekoa Proudfoot.
# Portions created by Kekoa Proudfoot are Copyright (C) 1998, 1999
# Kekoa Proudfoot. All Rights Reserved.

# specify environment before including the common stuff
BUILDING_HOST_UTILS = true
include ../../Makefile.common

FIRMDL3 = firmdl3$(EXT)
MKIMG = mkimg$(EXT)
MAN1 = firmdl3.1

ALL_TARGETS = ../$(FIRMDL3) $(MKIMG) 


all: fastdl.h $(ALL_TARGETS)
	@# nothing to do here but do it silently

$(MKIMG): mkimg.o srec.o
	$(CC) $^ -o $@ $(CFLAGS)

../$(FIRMDL3): firmdl.o srec.o rcx_comm.o
	$(CC) $^ -o $@ $(CFLAGS)

fastdl.h: $(MKIMG) fastdl.srec
	./$(MKIMG) fastdl.srec > $@

depend:
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

clean:
	rm -f *.o *~ *.bak

realclean: clean
	rm -f fastdl.h $(ALL_TARGETS)
	@rm -f install-stamp

.PHONY: realclean clean install depend all
