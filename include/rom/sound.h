/*! \file   include/rom/sound.h
    \brief  ROM Interface: RCX sound functions
    \author Markus L. Noga <markus@noga.de>

    \warning These functions will only work if ROM is allowed to handle
    the OCIA interrupt. system time, motor control and
    task management depend upon handling it themselves.
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
 */

#ifndef __rom_sound_h__
#define __rom_sound_h__

#ifdef  __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////////////////////////
//
// Functions
//
///////////////////////////////////////////////////////////////////////

//! play one of the system sounds.
/*! \param nr - the number of the system sound to be played
 *  \return Nothing
 *  NOTE: the system beep sound (#0) is the only system sound
 *   implemented at this time.
 *  \bug FIXME: register clobbers
*/
extern inline void sound_system(unsigned nr)
{
  __asm__ __volatile__(
      "push %0\n"
      "mov.w #0x4003,r6\n"
      "jsr @sound_system\n"
      "adds #0x2,sp\n"
      : // output
      :"r"(nr)  // input
      :"r6", "cc", "memory" // clobbered
  );
}

//! is a sound playing?
/*!  \return  T/F where T means a sound is playing
*/
extern inline int sound_playing(void)
{
  unsigned rc;
  __asm__ __volatile__(
      "mov.w r7,r6\n"
      "push r6\n"
      "mov.w #0x700c,r6\n"
      "jsr @sound_playing\n"
      "adds #0x2,sp\n"
      "mov.w @r7,%0\n"
      :"=r"(rc) // output
      : // input
      :"r6", "cc", "memory" // clobbered
  );

  return rc;
}

#ifdef  __cplusplus
}
#endif

#endif // __rom_sound_h__
