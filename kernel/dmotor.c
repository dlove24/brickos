/*! \file   dmotor.c
    \brief  Implementation: direct motor control
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
 *                  Lou Sortman <lou (at) sunsite (dot) unc (dot) edu>
 */

#include <sys/dmotor.h>
#include <dlcd.h>

#ifdef CONF_DMOTOR

#include <sys/h8.h>
#include <sys/irq.h>

///////////////////////////////////////////////////////////////////////////////
//
// Variables
//
///////////////////////////////////////////////////////////////////////////////

//! motor drive patterns
/*! to be indexed with MotorDirections
    \sa MotorDirections
*/
#ifdef CONF_DMOTOR_HOLD
const unsigned char dm_a_pattern[]={0xc0,0x40,0x80,0x00},
		    dm_b_pattern[]={0x0c,0x04,0x08,0x00},
		    dm_c_pattern[]={0x03,0x01,0x02,0x00};
#else
const unsigned char dm_a_pattern[]={0x00,0x80,0x40,0xc0},
		    dm_b_pattern[]={0x00,0x08,0x04,0x0c},
		    dm_c_pattern[]={0x00,0x02,0x01,0x03};
#endif

MotorState dm_a,                  //!< motor A state
           dm_b,                  //!< motor B state
           dm_c;                  //!< motor C state

        
///////////////////////////////////////////////////////////////////////////////
//
// Functions
//
///////////////////////////////////////////////////////////////////////////////

//! direct motor output handler
/*! called by system timer in the 16bit timer OCIA irq
*/
extern void dm_handler(void);
#ifndef DOXYGEN_SHOULD_SKIP_THIS
__asm__("
.section .text.hi
.align 1
.global _dm_handler
_dm_handler:
		; r6 saved by ROM
                ; r0 saved by systime_handler
"
#ifdef CONF_DMOTOR_HOLD
"		mov.b #0xcf,r6l			; r6l is output\n"
#else
"     	      	sub.w r6,r6   	      	      	; r6l is output\n"
#endif
"               ; we simultaneously load delta (r0h) and sum (r0l)
                ; this depends on byte order, but the H8 will stay MSB
                ; and the resulting code is efficient and compact.
                
                ; motor A
                
                mov.w   @_dm_a,r0
		add.b	#1,r0h			; maps 255 to 256
		dec.b	r0h
                addx.b	r0h,r0l                 ; add delta to sum
                bcc     dm0                     ; sum overflow?
		  mov.b	@_dm_a+2,r6h            ; -> output drive pattern
		  xor.b	r6h,r6l
            dm0:mov.b   r0l,@_dm_a+1            ; save sum

                ; motor B
                
                mov.w   @_dm_b,r0
		add.b	#1,r0h			; maps 255 to 256
		dec.b	r0h
                addx.b	r0h,r0l                 ; add delta to sum
                bcc     dm1                     ; sum overflow?
		  mov.b	@_dm_b+2,r6h            ; -> output drive pattern
		  xor.b	r6h,r6l
            dm1:mov.b   r0l,@_dm_b+1            ; save sum

                ; motor C
                
                mov.w   @_dm_c,r0
		add.b	#1,r0h			; maps 255 to 256
		dec.b	r0h
                addx.b	r0h,r0l                 ; add delta to sum
                bcc     dm2                     ; sum overflow?
		  mov.b	@_dm_c+2,r6h            ; -> output drive pattern
		  xor.b	r6h,r6l
            dm2:mov.b   r0l,@_dm_c+1            ; save sum

		; driver chip
                  
		mov.b	r6l,@_motor_controller:8	; output motor waveform
		
		rts		
	");
#endif // DOXYGEN_SHOULD_SKIP_THIS
	
		
//! initialize motors
//
void dm_init(void) {
  dm_shutdown();				// shutdown hardware
}


//! shutdown motors
//
void dm_shutdown(void) {
  motor_a_dir(off);			// initialize driver data
  motor_b_dir(off);
  motor_c_dir(off);

  motor_a_speed(MAX_SPEED);
  motor_b_speed(MAX_SPEED);
  motor_c_speed(MAX_SPEED);

  motor_controller=0x00;		// shutdown hardware
}

#ifdef CONF_VIS
/*
** Define non-inline versions to display arrows
*/

void motor_a_dir(MotorDirection dir)
{
  dm_a.dir = dm_a_pattern[dir];
  dlcd_hide(LCD_A_LEFT);
  dlcd_hide(LCD_A_RIGHT);
  if (dir == fwd || dir == brake)
    dlcd_show(LCD_A_RIGHT);
  else if (dir == rev || dir == brake)
    dlcd_show(LCD_A_LEFT);
}

void motor_b_dir(MotorDirection dir)
{
  dm_b.dir = dm_b_pattern[dir];
  dlcd_hide(LCD_B_LEFT);
  dlcd_hide(LCD_B_RIGHT);
  if (dir == fwd || dir == brake)
    dlcd_show(LCD_B_RIGHT);
  else if (dir == rev || dir == brake)
    dlcd_show(LCD_B_LEFT);
}

void motor_c_dir(MotorDirection dir)
{
  dm_c.dir = dm_c_pattern[dir];
  dlcd_hide(LCD_C_LEFT);
  dlcd_hide(LCD_C_RIGHT);
  if (dir == fwd || dir == brake)
    dlcd_show(LCD_C_RIGHT);
  else if (dir == rev || dir == brake)
    dlcd_show(LCD_C_LEFT);
}

#endif // ifdef CONF_VIS

#endif // CONF_DMOTOR
