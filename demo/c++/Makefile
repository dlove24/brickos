### ==========================================================================
###  $Id: Makefile,v 1.9 2002/10/23 17:42:44 stephmo Exp $
###  FILE: demo/c++/Makefile - make the C++ Language Demo Programs
###  brickOS - the independent LEGO Mindstorms OS
### --------------------------------------------------------------------------

# #LIBDIR#         ###   DO NOT REMOVE THIS LINE  ###
KERNEL = $(BRICKOS_ROOT)/boot/brickOS

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

include ../../Makefile.common
include ../../Makefile.user


all:: $(PROGRAMS)
	@# nothing to do here but do it silently

.depend: *.[cC]
	$(MAKEDEPEND) *.[cC] > .depend

depend:: .depend
	@# nothing to do here but do it silently

#  NOTE: --format=1 is not supported on Linux ([ce]tags from emacs2[01] packages)
#   please set in your own environment
tag:
	-ctags *.[cC] $(BRICKOS_ROOT)/include/*.h $(BRICKOS_ROOT)/include/*/*.h
	-etags *.[cC] $(BRICKOS_ROOT)/include/*.h $(BRICKOS_ROOT)/include/*/*.h 

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


### --------------------------------------------------------------------------
###                     End of FILE: demo/c++/Makefile
### ==========================================================================
