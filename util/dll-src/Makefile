# Makefile - keep the upper hand.
#
# 2000.03.07 - Paolo Masetti <paolo.masetti@itlug.org>
#
#	- Conditional make for Windows NT (cygnwin)
#	- $CC defined
#	- Martin Corneluis (cornelius@csd.de) fix for lnp includes
#	  see http://www.lugnet.com/robotics/rcx/legos/?n=498 for details
#

include ../Makefile.common

INSTALL_DIR = ..
EXE1 = dll$(EXT)
MAN1 = dll.1
TARGET1 = $(INSTALL_DIR)/$(EXE1)
SRCS1 = loader.c rcxtty.c keepalive.c ../../kernel/lnp.c lx.c
OBJS1 = $(notdir $(SRCS1:.c=.o))

EXE2 = makelx$(EXT)
TARGET2 = $(INSTALL_DIR)/$(EXE2)
SRCS2 = convert.c srec.c srecload.c lx.c
OBJS2 = $(SRCS2:.c=.o)

EXE3 = genlds$(EXT)
TARGET3 = $(INSTALL_DIR)/$(EXE3)
EXE4 = fixdeps$(EXT)
TARGET4 = $(INSTALL_DIR)/$(EXE4)

SINGLE_SRC_TARGETS = $(TARGET3) $(TARGET4)
ALL_TARGETS        = $(TARGET1) $(TARGET2) $(SINGLE_SRC_TARGETS)
LIBS=

#
# Martin Cornelius solution to include problems (avoid -I/usr/include)
#
CFLAGS+=-I. -I../../include/lnp

all:  $(ALL_TARGETS)
	@# nothing to be done, but silently

clean:
	rm -f *.o *~ *.bak 

realclean: clean
	rm -f $(ALL_TARGETS) 
	@rm -f .depend install-stamp

.depend:
	$(CC) -M $(CFLAGS) -c $(SRCS1) $(SRCS2) >.depend

depend: .depend
	@# nothing to be done, but silently

install: install-stamp
	@# nothing to be done, but silently

install-stamp: $(ALL_TARGETS)
	cp -f $(TARGET1) $(bindir)/$(EXE1)
	@if [ ! -d ${pkglibdir} ]; then \
		mkdir -p ${pkglibdir}; \
	fi
	cp -f $(TARGET2) $(pkglibdir)/$(EXE2)
	cp -f $(TARGET3) $(pkglibdir)/$(EXE3)
	cp -f $(TARGET4) $(pkglibdir)/$(EXE4)
	@if [ ! -d ${mandir}/man1 ]; then \
		mkdir -p ${mandir}/man1; \
	fi
	cp -f $(MAN1) $(mandir)/man1/$(MAN1)
	@touch $@

$(TARGET1):  $(OBJS1)
	$(CC) -o $@ $(OBJS1) $(LIBS) $(CFLAGS)

$(TARGET2):  $(OBJS2)
	$(CC) -o $@ $(OBJS2) $(LIBS) $(CFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

%.o: ../../kernel/%.c
	$(CC) -o $@ -c $< $(CFLAGS)

../%$(EXT): %.c
	$(CC) -o $@ $< $(CFLAGS)

.PHONY: depend all clean realclean install
# DO NOT DELETE
