/*! \file   include/dsound.h
    \brief  direct sound access
    \author Markus L. Noga <markus@noga.de>
 */

/*
 *  The contents of this file are subject to the Mozilla Public License
 *  Version 1.0 (the "License"); you may not use this file except in
 *  compliance with the License. You may obtain a copy of the License
 *  at http://www.mozilla.org/MPL/
 *
 *  Software distributed under the License is distributed on an "AS IS"
 *  basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 *  the License for the specific language governing rights and
 *  limitations under the License.
 *
 *  The Original Code is legOS code, released October 17, 1999.
 *
 *  The Initial Developer of the Original Code is Markus L. Noga.
 *  Portions created by Markus L. Noga are Copyright (C) 1999
 *  Markus L. Noga. All Rights Reserved.
 *
 *  Contributor(s): Markus L. Noga <markus@noga.de>
 *                  Froods <froods@alphalink.com.au>
 */

#ifndef __dsound_h__
#define __dsound_h__

#ifdef  __cplusplus
extern "C" {
#endif

#include <config.h>

#ifdef CONF_DSOUND

#include <time.h>
#include <unistd.h>

///////////////////////////////////////////////////////////////////////
//
// Definitions
//
///////////////////////////////////////////////////////////////////////

#ifndef DOXYGEN_SHOULD_SKIP_INTERNALS
//! a note
typedef struct {
  unsigned char pitch; 	    //!< note pitch, 0 ^= A_0 (~55 Hz)
  unsigned char length;     //!< note length in 1/16ths
} note_t;

#endif /* DOXYGEN_SHOULD_SKIP_INTERNALS */

//! note lengths
#define WHOLE            16
#define HALF             8
#define QUARTER          4
#define EIGHTH           2

//! note pitches
// PITCH_H is European equivalent to American B note.

#define PITCH_A0 	 0
#define PITCH_Am0 	 1
#define PITCH_H0 	 2
#define PITCH_C1 	 3
#define PITCH_Cm1 	 4
#define PITCH_D1 	 5
#define PITCH_Dm1 	 6
#define PITCH_E1 	 7
#define PITCH_F1 	 8
#define PITCH_Fm1 	 9
#define PITCH_G1 	 10
#define PITCH_Gm1 	 11
#define PITCH_A1 	 12
#define PITCH_Am1 	 13
#define PITCH_H1 	 14
#define PITCH_C2 	 15
#define PITCH_Cm2 	 16
#define PITCH_D2 	 17
#define PITCH_Dm2 	 18
#define PITCH_E2 	 19
#define PITCH_F2 	 20
#define PITCH_Fm2 	 21
#define PITCH_G2 	 22
#define PITCH_Gm2 	 23
#define PITCH_A2 	 24
#define PITCH_Am2 	 25
#define PITCH_H2 	 26
#define PITCH_C3 	 27
#define PITCH_Cm3 	 28
#define PITCH_D3 	 29
#define PITCH_Dm3 	 30
#define PITCH_E3 	 31
#define PITCH_F3 	 32
#define PITCH_Fm3 	 33
#define PITCH_G3 	 34
#define PITCH_Gm3 	 35
#define PITCH_A3 	 36
#define PITCH_Am3 	 37
#define PITCH_H3 	 38
#define PITCH_C4 	 39
#define PITCH_Cm4 	 40
#define PITCH_D4 	 41
#define PITCH_Dm4 	 42
#define PITCH_E4 	 43
#define PITCH_F4 	 44
#define PITCH_Fm4 	 45
#define PITCH_G4 	 46
#define PITCH_Gm4 	 47
#define PITCH_A4 	 48
#define PITCH_Am4 	 49
#define PITCH_H4 	 50
#define PITCH_C5 	 51
#define PITCH_Cm5 	 52
#define PITCH_D5 	 53
#define PITCH_Dm5 	 54
#define PITCH_E5 	 55
#define PITCH_F5 	 56
#define PITCH_Fm5 	 57
#define PITCH_G5 	 58
#define PITCH_Gm5 	 59
#define PITCH_A5 	 60
#define PITCH_Am5 	 61
#define PITCH_H5 	 62
#define PITCH_C6 	 63
#define PITCH_Cm6 	 64
#define PITCH_D6 	 65
#define PITCH_Dm6 	 66
#define PITCH_E6 	 67
#define PITCH_F6 	 68
#define PITCH_Fm6 	 69
#define PITCH_G6 	 70
#define PITCH_Gm6 	 71
#define PITCH_A6 	 72
#define PITCH_Am6 	 73
#define PITCH_H6 	 74
#define PITCH_C7 	 75
#define PITCH_Cm7 	 76
#define PITCH_D7 	 77
#define PITCH_Dm7 	 78
#define PITCH_E7 	 79
#define PITCH_F7 	 80
#define PITCH_Fm7 	 81
#define PITCH_G7 	 82
#define PITCH_Gm7 	 83
#define PITCH_A7 	 84
#define PITCH_Am7 	 85
#define PITCH_H7 	 86
#define PITCH_C8 	 87
#define PITCH_Cm8 	 88
#define PITCH_D8 	 89
#define PITCH_Dm8 	 90
#define PITCH_E8 	 91
#define PITCH_F8 	 92
#define PITCH_Fm8 	 93
#define PITCH_G8 	 94
#define PITCH_Gm8 	 95
#define PITCH_A8 	 96

#define PITCH_PAUSE   	 97
#define PITCH_MAX     	 98
#define PITCH_END     	 255


//! system sounds
#define DSOUND_BEEP   	 0

#define DSOUND_SYS_MAX   1


//! default duration of 1/16th note in ms
#define DSOUND_DEFAULT_16th_ms  200

//! default duration internote spacing in ms
#define DSOUND_DEFAULT_internote_ms  15

///////////////////////////////////////////////////////////////////////
//
// Variables
//
///////////////////////////////////////////////////////////////////////

extern unsigned dsound_16th_ms;   	      	 //!< length of 1/16 note in ms
extern unsigned dsound_internote_ms;  	      	 //!< length of internote spacing in ms
extern volatile note_t *dsound_next_note;     	 //!< pointer to current note
extern volatile time_t dsound_next_time;      	 //!< when to play next note

extern const note_t *dsound_system_sounds[];  	 //!< system sound data


///////////////////////////////////////////////////////////////////////
//
// User functions
//
///////////////////////////////////////////////////////////////////////

//! play a sequence of notes
static inline void dsound_play(const note_t *notes) {
  dsound_next_note=(volatile note_t*) notes;
  dsound_next_time=0;
}

//! play a system sound
static inline void dsound_system(unsigned nr) {
  if(nr<DSOUND_SYS_MAX)
    dsound_play(dsound_system_sounds[nr]);
}

//! set duration of a 16th note in ms
static inline void dsound_set_duration(unsigned duration) {
  dsound_16th_ms=duration;
}

//! set duration of inter-note spacing (subtracted from note duration)
/*! set to 0 for perfect legato.
*/
static inline void dsound_set_internote(unsigned duration) {
  dsound_internote_ms=duration;
}

//! returns nonzero value if a sound is playing
static inline int dsound_playing(void) {
  return dsound_next_note!=0;
}

//! sound finished event wakeup function
extern wakeup_t dsound_finished(wakeup_t data);

//! stop playing sound
extern void dsound_stop(void);

#endif // CONF_DSOUND

#ifdef  __cplusplus
}
#endif

#endif	/* __dsound_h__ */
