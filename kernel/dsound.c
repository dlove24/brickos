/*! \file   dsound.c
    \brief  Implementation: direct sound control
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
 *                  Michael Nielsen <mic@daimi.aau.dk>
 *    	      	    Kyosuke Ishikawa <kyosuke@da2.so-net.ne.jp>
 *                  Martin Cornelius <Martin.Cornelius@t-online.de>
 */

#include <sys/dsound.h>

#ifdef CONF_DSOUND

#include <sys/bitops.h>
#include <sys/h8.h>
#include <sys/irq.h>

#include <conio.h>
#include <sys/tm.h>

///////////////////////////////////////////////////////////////////////////////
//
// Variables
//
///////////////////////////////////////////////////////////////////////////////

//! note pitch -> frequency generator lookup table, index 0 ^= A0
static const unsigned pitch2freq[]={
  0x8d03, 0x8603, 0x7d03, 0x7703, 0x7003, 0x6a03, 0x6303, 0x5e03,
  0x5903, 0x5403, 0x4f03, 0x4a03, 0x4603, 0x4203, 0xfd83, 0xee83,
  0xe083, 0xd483, 0xc783, 0xbc83, 0xb283, 0xa883, 0x9e83, 0x9583,
  0x8d83, 0x8583, 0x7e83, 0x7683, 0x7083, 0x6983, 0x6383, 0x5e83,
  0x5983, 0x5383, 0x4f83, 0x4a83, 0x4683, 0x4283, 0xfc02, 0xee02,
  0xe102, 0xd402, 0xc802, 0xbd02, 0xb202, 0xa802, 0x9e02, 0x9502,
  0x8d02, 0x8502, 0xfc82, 0xee82, 0xe082, 0xd482, 0xc882, 0xbd82,
  0xb282, 0xa882, 0x9e82, 0x9682, 0x8d82, 0x8582, 0x7e82, 0x7682,
  0x7082, 0x6982, 0x6382, 0x5e82, 0x5882, 0x5382, 0x4f82, 0x4a82,
  0x4682, 0x4282, 0xfc01, 0xee01, 0xe001, 0xd401, 0xc801, 0xbd01,
  0xb201, 0xa801, 0x9e01, 0x9501, 0x8d01, 0x8501, 0x7e01, 0x7601,
  0x7001, 0x6901, 0x6301, 0x5e01, 0x5801, 0x5301, 0x4f01, 0x4a01,
  0x4601
};

//! single beep
static const note_t sys_beep[]={
  {PITCH_A4 , 1}, {PITCH_END, 0}
};

//! system sound data
const note_t *dsound_system_sounds[]={
  sys_beep
};
    
unsigned dsound_16th_ms;   	      	      	//!< length of 1/16 note in ms
unsigned dsound_internote_ms; 		      	//!< length of internote spacing in ms
volatile note_t *dsound_next_note;     	      	//!< pointer to current note
volatile time_t dsound_next_time;      	      	//!< when to play next note

static volatile int internote; 	      	      	//!< internote delay flag


//////////////////////////////////////////////////////////////////////////////
//
// Internal Functions
//
///////////////////////////////////////////////////////////////////////////////

//! start playing a given frequency
static inline void play_freq(unsigned freq) {
  unsigned char CKSmask = freq & 0xff;
  unsigned char match = freq >> 8;

  T0_CR  = 0x00;                 // timer off
  T0_CNT = 0x00;	         // counter reset
  
#if 0  
  bit_load(CKSmask,0x7);      	 // set ICKS0
  bit_store(&STCR,0x0);
#else  
  if (CKSmask & 0x80)
      STCR |=  0x01;             // ICKS0 = 1
  else
      STCR &= ~0x01;             // ICKS0 = 0
#endif
  
  T0_CORA = match;               // set compare match A
  T0_CR   = CR_CLEAR_ON_A | (CKSmask &0x3);
}

//! start playing a pause (aka stop playing freq)
static inline void play_pause() {
  T0_CR  = 0x00;      	      	 // timer 0 off
}

 
//////////////////////////////////////////////////////////////////////////////
//
// System functions
//
///////////////////////////////////////////////////////////////////////////////

//! sound handler, called from system timer interrupt
#ifdef CONF_RCX_COMPILER
void dsound_handler(void) {
#else
HANDLER_WRAPPER("dsound_handler","dsound_core");
//! sound core, called from ????
void dsound_core(void) {
#endif
  if (get_system_up_time() >= dsound_next_time) {
    
    if(internote) {
      play_pause();
      dsound_next_time = get_system_up_time() + dsound_internote_ms;
      
      internote=0;
      return;
    } 
         
    if(dsound_next_note) {
      unsigned char pitch =dsound_next_note->pitch;
      
      if(pitch<PITCH_MAX) {
	if(pitch!=PITCH_PAUSE)
  	  play_freq(pitch2freq[pitch]);
	else
	  play_pause();
	
	dsound_next_time = get_system_up_time() + dsound_16th_ms * dsound_next_note->length
	                          - dsound_internote_ms;
	dsound_next_note++;
	internote=1;
	return;
      }
    }
    
    dsound_stop();
  }  
}

//! initialize sound driver
void dsound_init() {
  dsound_16th_ms=DSOUND_DEFAULT_16th_ms;
  dsound_internote_ms=DSOUND_DEFAULT_internote_ms;
  dsound_stop();
  T0_CSR  = CSR_TOGGLE_ON_A;     // Output toggles on compare Match A
}
  
//! shutdown sound driver
void dsound_shutdown() {
  dsound_stop();
}


//////////////////////////////////////////////////////////////////////////////
//
// User functions
//
///////////////////////////////////////////////////////////////////////////////

//! stop playing sound
void dsound_stop(void) {
  play_pause();
  dsound_next_note=0;  
  dsound_next_time=0xffffffff;
  internote=0;
}

//! sound finished event wakeup function
wakeup_t dsound_finished(wakeup_t data) {
  return !dsound_playing();
}

#endif // CONF_DSOUND
