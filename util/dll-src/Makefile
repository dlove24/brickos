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

TARGET1=../dll$(EXT)
SRCS1=loader.c rcxtty.c keepalive.c ../../kernel/lnp.c lx.c
OBJS1=$(notdir $(SRCS1:.c=.o))

TARGET2=../makelx$(EXT)
SRCS2=convert.c srec.c srecload.c lx.c
OBJS2=$(SRCS2:.c=.o)

SINGLE_SRC_TARGETS=../genlds$(EXT) ../fixdeps$(EXT)

ALL_TARGETS=$(TARGET1) $(TARGET2) $(SINGLE_SRC_TARGETS)
LIBS=

#
# Martin Cornelius solution to include problems (avoid -I/usr/include)
#
CFLAGS+=-I. -I../../include/lnp

all:  $(ALL_TARGETS)

clean:
	rm -f *.o *~ *.bak $(ALL_TARGETS) .depend

depend:
	$(CC) -M $(CFLAGS) -c $(SRCS1) $(SRCS2) >.depend

$(TARGET1):  $(OBJS1)
	$(CC) -o $(TARGET1) $(OBJS1) $(LIBS) $(CFLAGS)

$(TARGET2):  $(OBJS2)
	$(CC) -o $(TARGET2) $(OBJS2) $(LIBS) $(CFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

%.o: ../../kernel/%.c
	$(CC) -o $@ -c $< $(CFLAGS)

../%$(EXT): %.c
	$(CC) -o $@ $< $(CFLAGS)

# DO NOT DELETE
