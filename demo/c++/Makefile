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
	raceTrack.lx

# extra dynamic sources
DOBJECTS= 

include $(ROOTDIR)Makefile.common
include $(ROOTDIR)Makefile.user

.PHONY: all html depend tag clean realclean

all: $(PROGRAMS)

# doc/html subdirectory
html:
	doxygen Doxyfile

depend .depend:
	$(MAKEDEPEND) *.[cC] > .depend

tag:
	ctags *.[cC] $(ROOTDIR)include/*.h $(ROOTDIR)include/*/*.h
	etags *.[cC] $(ROOTDIR)include/*.h $(ROOTDIR)include/*/*.h 

clean:
	rm -rf .depend* *.o *.dis *.dis2 *~ *.bak *.tgz *.s tags *.ds1 *.ds2 *.dmap *.dcoff *.srec *.map *.coff

realclean: clean
	rm -f *.lx

# depencencies
#
ifndef NODEPS
-include .depend
endif
