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

// Define an interface to sound, via dsound.h
// Methods include:
// int playing() see dsound_playing();
// void beep() causes a beeb
// void stop() stop playing
// void duration(const unsigned int len) see dsound_set_duration
// void internote(const unsigned int len) see dsound_set_internote

#include <dsound.h>

#if defined(CONF_DSOUND)

/**
 * @class Sound Sound.H <c++/Sound.H>
 *
 * Sound Generation Class interface
 *
 * @short Sound Generation interface
 * @see Motor, MotorPair
 */
class Sound {
public:
  static int playing() {return dsound_playing();}
  static void beep() {dsound_system(DSOUND_BEEP);}
  static void stop() {dsound_stop();}
  static void duration(const unsigned int len) {dsound_set_duration(len);}
  static void internote(const unsigned int len) {dsound_set_internote(len);}
};

#else
#warning Enable CONF_DSOUND to use Sound.H
#endif // CONF_DSOUND
#endif // _Sound_H_
