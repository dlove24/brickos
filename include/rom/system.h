/*! \file   include/rom/system.h
    \brief  ROM system control functions
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
 */

#ifndef __rom_system_h__
#define __rom_system_h__

#ifdef  __cplusplus
extern "C" {
#endif

#include <sys/irq.h>

///////////////////////////////////////////////////////////////////////
//
// Functions
//
///////////////////////////////////////////////////////////////////////

//! enters software standby mode.
extern inline void power_off(void)
{
  __asm__ __volatile__("
	push r6
	bset #7,@0xc4:8	 ; software standby mode
	jsr @ power_off  ; ROM call
      	pop r6
");
}

//! disables software standby mode so tm_idle_task() can use the sleep
//  instruction.
extern inline void power_init(void) {
  __asm__ __volatile__("
	push r6
	jsr @ power_init ; ROM call
	bclr #7,@0xc4:8  ; disable software standby
	pop r6
");
}

//! erases legOS, returning control to ROM.
extern void reset(void) __attribute__((noreturn));

extern inline void rom_reset(void) __attribute__((noreturn));
extern inline void rom_reset(void) {
  disable_irqs();
  reset();
}

#ifdef  __cplusplus
}
#endif

#endif // __rom_system_h__
