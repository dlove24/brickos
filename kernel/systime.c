/*! \file systime.c
    \brief  system time services
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
 *                  David Van Wagner <davevw@alumni.cse.ucsc.edu>
 */

/*
 *  2000.05.01 - Paolo Masetti <paolo.masetti@itlug.org>
 *
 * - Added battery indicator handler
 *
 *  2000.08.12 - Rossz Vámos-Wentworth <rossw@jps.net>
 *
 * - Added idle shutdown handler
 *
 */

#include <config.h>

#ifdef CONF_TIME

#include <sys/time.h>
#include <sys/h8.h>
#include <sys/irq.h>
#include <sys/dmotor.h>
#include <sys/dsound.h>
#include <sys/battery.h>
#ifdef CONF_AUTOSHUTOFF
#include <sys/timeout.h>
#endif

///////////////////////////////////////////////////////////////////////////////
//
// Global Variables
//
///////////////////////////////////////////////////////////////////////////////

//! current system time in ms
/*! \warning This is a 32 bit value which will overflow after 49.7 days
             of continuous operation.
*/
volatile time_t sys_time;

///////////////////////////////////////////////////////////////////////////////
//
// Internal Variables
//
///////////////////////////////////////////////////////////////////////////////

#ifdef CONF_TM
volatile unsigned char tm_timeslice;            //!< task time slice
volatile unsigned char tm_current_slice;        //!< current time remaining

void* tm_switcher_vector;                       //!< pointer to task switcher
#endif


///////////////////////////////////////////////////////////////////////////////
//
// Functions
//
///////////////////////////////////////////////////////////////////////////////

//! system time handler for the 16bit timer OCIA irq
/*! this is the pulse of the system.
    task switcher and motor driver calls are initiated here.
*/
extern void systime_handler(void);

__asm__("
.text
.align 1
.global _systime_handler
.global _systime_tm_return
_systime_handler:
               ; r6 saved by ROM

                push r0                         ; both motors & task
                                                ; switcher need this reg.

                ; increment system timer

                mov.w @_sys_time+2,r6           ; LSW -> r6
                add.b #0x1,r6l                  ; 16 bit: add 1
                addx  #0x0,r6h
                mov.w r6,@_sys_time+2
                bcc sys_nohigh                  ; speedup for 65535 cases

                  mov.w @_sys_time,r6           ; MSW -> r6
                  add.b #0x1,r6l
                  addx  #0x0,r6h
                  mov.w r6,@_sys_time
              sys_nohigh:
        "
#ifdef CONF_DMOTOR
        "
                jsr _dm_handler                 ; call motor driver
"
#endif

#ifdef CONF_DSOUND
        "
                jsr _dsound_handler             ; call sound handler
        "
#endif

#ifdef CONF_LNP
        "
                mov.w @_lnp_timeout_counter,r6  ; check LNP timeout counter
                subs #0x1,r6
                mov.w r6,r6                     ; subs doesn't change flags!
                bne sys_noreset
                
                  jsr _lnp_integrity_reset
                  mov.w @_lnp_timeout,r6        ; reset timeout

              sys_noreset:
                mov.w r6,@_lnp_timeout_counter
        "
#endif

#ifdef CONF_DKEY
        "
                jsr _dkey_handler
        "
#endif

#ifndef CONF_TM
#ifdef CONF_BATTERY_INDICATOR
        "
                mov.w @_battery_refresh_counter,r6
                subs #0x1,r6
                bne batt_norefresh

                  jsr _battery_refresh
                  mov.w @_battery_refresh_period,r6

              batt_norefresh:
                mov.w r6,@_battery_refresh_counter
        "
#endif
#endif

#ifdef CONF_AUTOSHUTOFF
        "
                mov.w @_auto_shutoff_counter,r6
                subs  #0x1,r6
                bne auto_notshutoff

                  jsr _autoshutoff_check
                  mov.w @_auto_shutoff_period,r6
                  
              auto_notshutoff:
                  mov.w r6,@_auto_shutoff_counter
        "
#endif

#ifdef CONF_VIS
        "
                mov.b @_vis_refresh_counter,r6l
                dec r6l
                bne vis_norefresh
                
                  jsr _vis_handler
                  mov.b @_vis_refresh_period,r6l
                  
              vis_norefresh:
                mov.b r6l,@_vis_refresh_counter
        "
#endif

#ifdef CONF_LCD_REFRESH
        "
                mov.b @_lcd_refresh_counter,r6l
                dec r6l
                bne lcd_norefresh
                
                  jsr _lcd_refresh_next_byte
                  mov.b @_lcd_refresh_period,r6l
                  
              lcd_norefresh:
                mov.b r6l,@_lcd_refresh_counter
        "
#endif

#ifdef CONF_TM
        "
                mov.b @_tm_current_slice,r6l
                dec r6l
                bne sys_noswitch                ; timeslice elapsed?

                  mov.w @_tm_switcher_vector,r6
                  jsr @r6                       ; call task switcher
                  
              _systime_tm_return:
                mov.b @_tm_timeslice,r6l        ; new timeslice

              sys_noswitch:
                mov.b r6l,@_tm_current_slice
        "
#endif
        "
                pop r0
                bclr  #3,@0x91:8                ; reset compare A IRQ flag
                rts
        "
);


//! initialize system timer
/*! task switcher initialized to empty handler
    motors turned off
*/
void systime_init(void) {
  systime_shutdown();                           // shutdown hardware

  sys_time=0l;                                  // init timer

#ifdef CONF_TM
  tm_current_slice=tm_timeslice=TM_DEFAULT_SLICE;
  tm_switcher_vector=&rom_dummy_handler;        // empty handler
#endif

#ifdef CONF_DMOTOR
  dm_shutdown();
#endif

  // configure 16-bit timer compare A IRQ
  // to occur every 1 ms, hook and enable it.
  //
  T_CSR =TCSR_OCA | TCSR_RESET_ON_A;
  T_CR  =TCR_CLOCK_32;
  T_OCR&=~TOCR_OCRB;
  T_OCRA=500;

  ocia_vector=&systime_handler;
  T_IER|=TIER_ENABLE_OCA;
}

//! shutdown system timer
/*! will also stop task switching and motors.
*/
void systime_shutdown(void) {
  T_IER&=~TIER_ENABLE_OCA;    // unhook compare A IRQ
}

#ifdef CONF_TM
//! set task switcher vector
/*! \param switcher the switcher
*/
void systime_set_switcher(void* switcher) {
  tm_switcher_vector=switcher;
}

//! set multitasking timeslice in ms
/*! \param slice the timeslice. must be at least 5ms.
*/
void systime_set_timeslice(unsigned char slice) {
  if(slice>5) {                    // some minimum value
    tm_timeslice=slice;
    if(tm_current_slice>tm_timeslice)
      tm_current_slice=tm_timeslice;
  }
}

#endif

#endif // CONF_TIME
