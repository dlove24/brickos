/*! \file   dkey.c
    \brief  Implementation: debounced key driver
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
 */

#include <dkey.h>

#ifdef CONF_DKEY

#include <unistd.h>
#include <sys/tm.h>

#ifdef CONF_AUTOSHUTOFF
#include <sys/timeout.h>
#endif

///////////////////////////////////////////////////////////////////////////////
//
// Global Variables
//
///////////////////////////////////////////////////////////////////////////////

volatile unsigned char dkey_multi;   	//! multi-key state
volatile unsigned char dkey;          	//! single key state

///////////////////////////////////////////////////////////////////////////////
//
// Internal Variables
//
///////////////////////////////////////////////////////////////////////////////

char dkey_timer __attribute__ ((unused)); //! debouncing timer

///////////////////////////////////////////////////////////////////////////////
//
// Functions
//
///////////////////////////////////////////////////////////////////////////////
#ifndef DOXYGEN_SHOULD_SKIP_THIS
__asm__("
.text
.align 1
.global _dkey_handler
_dkey_handler:
   mov.b @_dkey_timer,r6l	   ; check debouncing timer==0
   beq dkey_check

     dec r6l
     mov.b r6l,@_dkey_timer
     rts

dkey_check:
   sub.b r6l,r6l      	      	   ; generate button codes
      	      	      	      	   ; from PORT4/PORT7 in r6l
   mov.b @_PORT4,r6h
   bld #1,r6h
   bist #0,r6l
   bld #2,r6h
   bist #1,r6l

   mov.b @_PORT7,r6h
   bld #6,r6h
   bist #2,r6l
   bld #7,r6h
   bist #3,r6l

   mov.b @_dkey_multi,r6h
   xor.b r6l,r6h      	      	  ; create mask of changed positions in r6h
   beq dkey_same

     mov.b r6l,@_dkey_multi

     and.b r6h,r6l    	      	  ; mask out unchanged positions
     mov.b r6l,@_dkey

     mov.b #100,r6l  	      	  ; set debouncing timer
     mov.b r6l,@_dkey_timer

dkey_same:
   rts
");
#endif // DOXYGEN_SHOULD_SKIP_THIS

//! wakeup if any of the given keys is pressed.
//
wakeup_t dkey_pressed(wakeup_t data) {
#ifdef CONF_AUTOSHUTOFF
  if (idle_powerdown) { // if idle too long, say the OFF key was pressed
    dkey = KEY_ONOFF;
    return KEY_ONOFF;
  }
#endif
  return (dkey & (unsigned char)data);
}

//! wakeup if all of the given keys are released.
//
wakeup_t dkey_released(wakeup_t data) {
  return ! (dkey & (unsigned char)data);
}

//! get and return a single key press, after waiting for it to arrive
//
int getchar(void) {
  wait_event(dkey_released,KEY_ANY);
#ifdef CONF_AUTOSHUTOFF
  shutoff_restart();
#endif
  wait_event(dkey_pressed ,KEY_ANY);
  return dkey;
}

#endif // CONF_DKEY
