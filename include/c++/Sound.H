/*! \file   include/c++/Sound.H
    \brief  C++ Sound Class Interface
    \author Pat Welch (legOS@mousebrains.com)  
		 
    Defines interface to RCX-internal speaker and sound generation routines
*/
//
// The contents of this file are subject to the Mozilla Public License
// Version 1.0 (the "License"); you may not use this file except in
// compliance with the License. You may obtain a copy of the License
// at http://www.mozilla.org/MPL/
// 
// Software distributed under the License is distributed on an "AS IS"
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
// the License for the specific language governing rights and
// limitations under the License.
//
// This software was developed as part of the legOS project.
//
// Contributor: Pat Welch (legOS@mousebrains.com)

#ifndef _Sound_H_
#define _Sound_H_

#include <config.h>
#include <dsound.h>

#if defined(CONF_DSOUND)

/**
 * \class Sound Sound.H c++/Sound.H
 * Sound generation interface.
 * Sound Generation Class interface
 *
 * \see The other control classes: Motor, MotorPair, Lamp
 */
class Sound {
public:
  /**
   * Play a list of notes (song).
   * \li to poll for playing complete try Sound::playing()
   * \li to wait for playing complete try dsound_finished()
   * \param notes pointer to an array of note_t elements
   * \return Nothing
   */
  static void play(const note_t *notes) {return dsound_play(notes);}
  /**
   * Determine if a sound is currently playing
   * \return T/F where T means a sound is playing
   * \see dsound_finished()
   */
  static int playing() {return dsound_playing();}
  /**
   * Play the system_beep sound
   * \return Nothing
   */
  static void beep() {dsound_system(DSOUND_BEEP);}
  /**
   * Stop a currently playing sound
   * \return Nothing
   */
  static void stop() {dsound_stop();}
  /**
   * Set the default duration
   * \param len the duration in mSec?
   * \return Nothing
   * \todo get the real scoop here, then provde better description
   */
  static void duration(const unsigned int len) {dsound_set_duration(len);}
  /**
   * Set the amount of time between notes
   * \param len the duration in mSec?
   * \return Nothing
   * \todo get the real scoop here, then provde better description
   */
  static void internote(const unsigned int len) {dsound_set_internote(len);}
};

#else
#warning Enable CONF_DSOUND to use Sound.H
#endif // CONF_DSOUND
#endif // _Sound_H_
