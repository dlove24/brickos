/*! \file   dsensor.c
    \brief  Implementation: direct sensor access
    \author Markus L. Noga <markus@noga.de>
*/

/*
 *  The contents of this file are subject to the Mozilla Public License
 *  Version 1.0 (the "License"); you may not use this file except in
 *  compliance with the License. You may obtain a copy of the License at
 *  http://www.mozilla.org/MPL/
 *
 *  Software distributed under the License is distributed on an "AS IS"
 *  basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 *  License for the specific language governing rights and limitations
 *  under the License.
 *
 *  The Original Code is legOS code, released October 17, 1999.
 *
 *  The Initial Developer of the Original Code is Markus L. Noga.
 *  Portions created by Markus L. Noga are Copyright (C) 1999
 *  Markus L. Noga. All Rights Reserved.
 *
 *  Contributor(s): Markus L. Noga <markus@noga.de>
 *                  Eric Habnerfeller <ehaberfe@atitech.ca>
 *                  Lou Sortman <lou@sunsite.unc.edu>
 */

/*
 *  2000.03.11 - Paolo Masetti <paolo.masetti@itlug.org>
 *
 *	- Included a fix for rotation sensor posted by "Ben Jackson"
 *        on lugnet.robotics.rcx.legos
 *
 *  2000.04.30 - Paolo Masetti <paolo.masetti@itlug.org>
 *
 *	- ISR Reading routine fix to make read values stable.
 *	- Fixed rotation sensor status table values to avoid offset problems.
 *
 *  2000.09.06 - Jochen Hoenicke <jochen@gnu.org>
 *
 *	- Added velocity calculation for rotation sensor.
 */

#include <dsensor.h>

#ifdef CONF_DSENSOR

#include <sys/h8.h>
#include <sys/irq.h>
#include <sys/bitops.h>
#include <rom/registers.h>
#include <unistd.h>
#include <conio.h>

///////////////////////////////////////////////////////////////////////////////
//
// Definitions
//
///////////////////////////////////////////////////////////////////////////////

#define DS_ALL_ACTIVE         0x07             //!< all sensors active mode
#define DS_ALL_PASSIVE        (~DS_ALL_ACTIVE) //!< all sensors passive mode

///////////////////////////////////////////////////////////////////////////////
//
// Variables
//
///////////////////////////////////////////////////////////////////////////////

volatile unsigned char ds_channel;        //!< current A/D channel

unsigned char ds_activation;              //!< channel bitmask. 1-> active

#ifdef CONF_DSENSOR_ROTATION
unsigned char ds_rotation;                //!< channel bitmask. 1-> rotation

volatile int ds_rotations[3];             //!< sensor revolutions * 16

static signed char rotation_state[3];     //!< rotation state
static signed char rotation_new_state[3]; //!< proposed rotation state
static unsigned int state_duration[3];    //!< proposed rotation state duration

#ifdef CONF_DSENSOR_VELOCITY
volatile int ds_velocities[3];            //!< sensor velocity
static unsigned int last_rotation[3];     //!< last time of rotation signal
static unsigned int next_rotation[3];     //!< rough upper estimatation of next signal time
static signed char rotation_dir[3];       //!< direction of last rotation
#endif

//! convert a/d values to rotation states
/*! Indexed with (value>>12).
    Invalid values yield negative states.
*/
static const signed char ad2state[16]={
  // 0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f	// (sensor value>>12)
    -1,-1,-1,-1,-1, 2, 2, 2, 3, 3, 3, 3, 1, 1, 1, 0	// New values to be used
							// with delayed read

//  -1,-1,-1,-1,-1,-1, 2, 2, 2, 3, 3, 3, 1, 1, 1, 0	// Old values: biased for
							// non-delayed read

};

//! convert state difference to revolution count change
/*! Indexed with (newstate-state)+3.
    Invalid differences yield zero change.
    Differences of magnitude two could have been acheived in either
    rotational sense, so their expected value is zero.
*/
static const signed char diff2change[7]={
  //-3 -2 -1  0  1  2  3      	      	      	      // newstate-state
     1, 0,-1, 0, 1, 0,-1
};

///////////////////////////////////////////////////////////////////////////////
//
// Functions
//
///////////////////////////////////////////////////////////////////////////////

//! set rotation to an absolute value
/*! \param sensor the sensor address, can be &SENSOR_1, &SENSOR_2 or &SENSOR_3
    \param pos    desired absolute position

    axis should be inert during the function call
*/
void ds_rotation_set(volatile unsigned *sensor,int pos) {
  if(sensor>=&AD_A && sensor<=&AD_C) {    // catch range violations
    unsigned channel=(unsigned) (sensor-&AD_A);
    signed char state=ad2state[(*sensor)>>12];

    if(state<0)
      state=0;

    rotation_state[channel]=state;
    rotation_new_state[channel] = -1;
    state_duration[channel]=0;
    ds_rotations[channel]=pos;            // reset counter

  }
}

//! process rotation sensor on current A/D channel
/*! \sa ds_channel current channel (global input value)
*/
void ds_rotation_handler() {
  unsigned    channel =ds_channel;
  unsigned    raw     =(*((&AD_A)+channel));
  signed char newstate=ad2state[raw>>12];

  if (newstate < 0)
    return;

  if (newstate == rotation_new_state[channel]) {
    if (++state_duration[channel] == 2) {
      signed char change = diff2change[newstate - rotation_state[channel] + 3];

      ds_rotations[channel] += change;

#ifdef CONF_DSENSOR_VELOCITY
      {
	/* We only take the lowest 16 bits of sys_time.  We have to be
	 * a bit careful with wraparounds, but this is handled here.
	 */
	unsigned int time = (unsigned int) sys_time;
	if (change != rotation_dir[channel]) {
	  rotation_dir[channel] = change;
	  ds_velocities[channel] = 0;
	  last_rotation[channel] = time;
	  next_rotation[channel] = time + 1000;
	} else {
	  if (time == last_rotation[channel])
	    ds_velocities[channel] = 1000 * change;
	  else {
	    unsigned int time_diff = (time - last_rotation[channel]);
	    if (time_diff > 1000) {
	      rotation_dir[channel] = 0;
	      ds_velocities[channel] = 0;
	    } else {
	      int speed = 1000 / time_diff;
	      ds_velocities[channel] = change > 0 ? speed : -speed;
	      last_rotation[channel] = time;
	      next_rotation[channel] = time + time_diff * 3 / 2;
	    }
	  }
	}
      }
#endif

      rotation_state[channel] = newstate;
      rotation_new_state[channel] = -1;
    }
  } else if (newstate != rotation_state[channel]) {
    rotation_new_state[channel] = newstate;
    state_duration[channel] = 1;
#ifdef CONF_DSENSOR_VELOCITY
  } else {
    /* No rotation change, check if velocity measure timeouts. */
    unsigned int time = (unsigned int) sys_time;
    if (rotation_dir[channel] &&  
	((signed int) (time - next_rotation[channel])) >= 0) {
      unsigned int time_diff = (time - last_rotation[channel]);
      if (time_diff > 1000) {
	rotation_dir[channel] = 0;
	ds_velocities[channel] = 0;
      } else {  
	int speed = 1000 / time_diff;
	ds_velocities[channel] = rotation_dir[channel] > 0 ? speed : -speed;
	next_rotation[channel] = time + time_diff / 2;
      }
    }
#endif
  }

}
#endif // CONF_DSENSOR_ROTATION

//! sensor A/D conversion IRQ handler
//
extern void ds_handler(void);
#ifndef DOXYGEN_SHOULD_SKIP_THIS
__asm__("
.text
.align 1
_ds_handler:
   ; r6 saved by ROM

   mov.b @_ds_channel,r6l	; r6l = current channel

   mov.b @_ds_activation,r6h	; r6h = activation bitmask
   btst r6l,r6h			; activate output?
   beq ds_noset
       bset r6l,@_PORT6:8	; activate output of last port scanned
 ds_noset:
        "
#ifdef CONF_DSENSOR_ROTATION
        "
   mov.b @_ds_rotation,r6h	; r6h = rotation bitmask
   btst r6l,r6h			; process rotation sensor?
   beq ds_norot

     push r0			; save r0..r3
     push r1
     push r2
     push r3			; r4..r6 saved by gcc if necessary

     jsr _ds_rotation_handler	; process rotation sensor

     pop r3
     pop r2
     pop r1
     pop r0
 ds_norot:
        "
#endif
        "
   inc r6l			; next channel
   and #0x03,r6l		; limit to 0-3
   bclr r6l,@_PORT6:8		; set output inactive for reading

   ; The settle time for reading the value from active sensor start here

   ; moved here for helping timing problems
   mov.b r6l,@_ds_channel	; store next channel

   ; Added a delay loop for sensor settle time

   mov.b #0x04, r6h		; delay loop
settle:
   nop				; each nop is a 2 state clock delay
   dec.b r6h			; 2 states ?
   bne settle			; 4 states

   ; Total loop delay 32 states (?)

   mov.b @_AD_CSR:8,r6h		; r6h = A/D CSR
   and.b #0x7c,r6h		; reset scanmode and channel num
   or.b  r6l,r6h		; scan next channel
   mov.b r6h,@_AD_CSR:8		; put r6h back on A/D CSR

   ; The settle time for reading the value from active sensor finish here

   bset #0x5,@_AD_CSR:8		; go!

   rts
");
#endif // DOXYGEN_SHOULD_SKIP_THIS


//! initialize sensor a/d conversion
/*! all sensors set to passive mode
    rotation tracking disabled
*/
void ds_init(void) {
  rom_port6_ddr|=DS_ALL_ACTIVE;         // notify ROM we are using
  PORT6_DDR     =rom_port6_ddr;         // PORT6 bit 0..2 as outputs

  ds_activation=0;                      // all sensors passive
  ds_channel   =0;			// start on channel 0

#ifdef CONF_DSENSOR_ROTATION
  ds_rotation  =0;                      // rotation tracking disabled
#endif

  ad_vector=&ds_handler;		// setup IRQ handler
  AD_CR &=~ADCR_EXTERN;
  AD_CSR =ADCSR_TIME_266 | ADCSR_GROUP_0 | ADCSR_AN_0  |
          ADCSR_ENABLE_IRQ | ADCSR_START;

#ifdef CONF_CONIO
    delay(10);				// wait for initial A/D
#else
# warning "Rotation initialization might fail."
#endif
}


//! shutdown sensor a/d conversion
/*! all sensors set to passive mode
*/
void ds_shutdown(void) {
  AD_CSR=0x00;
  PORT6        &=DS_ALL_PASSIVE;
  rom_port6_ddr&=DS_ALL_PASSIVE;
  PORT6_DDR     =rom_port6_ddr;
}

#endif  // CONF_DSENSOR
