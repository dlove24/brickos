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
__asm__("\n\
.text\n\
.align 1\n\
.global _dkey_handler\n\
_dkey_handler:\n\
   mov.b @_dkey_timer,r6l	   ; check debouncing timer==0\n\
   beq dkey_check\n\
\n\
     dec r6l\n\
     mov.b r6l,@_dkey_timer\n\
     rts\n\
\n\
dkey_check:\n\
   sub.b r6l,r6l      	      	   ; generate button codes\n\
      	      	      	      	   ; from PORT4/PORT7 in r6l\n\
   mov.b @_PORT4,r6h\n\
   bld #1,r6h\n\
   bist #0,r6l\n\
   bld #2,r6h\n\
   bist #1,r6l\n\
\n\
   mov.b @_PORT7,r6h\n\
   bld #6,r6h\n\
   bist #2,r6l\n\
   bld #7,r6h\n\
   bist #3,r6l\n\
\n\
   mov.b @_dkey_multi,r6h\n\
   xor.b r6l,r6h      	      	  ; create mask of changed positions in r6h\n\
   beq dkey_same\n\
\n\
     mov.b r6l,@_dkey_multi\n\
\n\
     and.b r6h,r6l    	      	  ; mask out unchanged positions\n\
     mov.b r6l,@_dkey\n\
\n\
     mov.b #100,r6l  	      	  ; set debouncing timer\n\
     mov.b r6l,@_dkey_timer\n\
\n\
dkey_same:\n\
   rts\n\
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
