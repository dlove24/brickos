##
## brickOS - the independent LEGO Mindstorms OS
## Makefile - allows you to keep the upper hand
## (c) 1998, 1999 by Markus L. Noga <markus@noga.de>    
##

ROOTDIR = ../../

KERNEL = $(ROOTDIR)boot/brickOS

PROGRAMS = \
	rover.lx \
	batt.lx sensor.lx lightSensor.lx \
	rotationSensor.lx \
	rotation.velocity.Sensor.lx \
	touchSensor.lx \
	touch.pressed.Sensor.lx \
	motor.lx sound.lx activeSensor.lx temperatureSensor.lx \
	raceTrack.lx lampTest.lx

# extra dynamic sources
DOBJECTS= 

include $(ROOTDIR)Makefile.common
include $(ROOTDIR)Makefile.user


all: $(PROGRAMS)
	@# nothing to do here but do it silently

.depend: *.[cC]
	$(MAKEDEPEND) *.[cC] > .depend

depend: .depend

#  NOTE: --format=1 is not supported on Linux ([ce]tags from emacs2[01] packages)
#   please set in your own environment
tag:
	-ctags *.[cC] $(ROOTDIR)include/*.h $(ROOTDIR)include/*/*.h
	-etags *.[cC] $(ROOTDIR)include/*.h $(ROOTDIR)include/*/*.h 

clean:
	rm -f *.o *.dis *.dis2 *~ *.bak *.tgz *.s *.ds1 *.ds2 *.dmap *.dcoff
	rm -f *.srec *.map *.coff

realclean: clean
	rm -f *.lx .depend tags TAGS

.PHONY: all depend tag clean realclean

# depencencies
#
ifndef NODEPS
-include .depend
endif
