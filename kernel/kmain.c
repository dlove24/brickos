/*! \file kmain.c
    \brief  Implementaion: Main kernel loop
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
 *                  Kekoa Proudfoot <kekoa@graphics.stanford.edu>
 *                  Lou Sortman <lou (at) sunsite (dot) unc (dot) edu>
 */

#include <sys/mm.h>
#include <sys/time.h>
#include <sys/tm.h>
#include <sys/irq.h>
#include <sys/lcd.h>
#include <sys/dsensor.h>
#include <sys/dmotor.h>
#include <sys/dsound.h>
#include <sys/lnp.h>
#include <sys/lnp-logical.h>
#include <sys/program.h>
#ifdef CONF_AUTOSHUTOFF
#include <sys/timeout.h>
#endif
#include <rom/system.h>

#include <dbutton.h>

#include <string.h>
#include <conio.h>
#include <unistd.h>
///////////////////////////////////////////////////////////////////////////////
//
// Variables
//
///////////////////////////////////////////////////////////////////////////////

//! firmware recognition string
/*! the ROM checks for this string when validating new firmware
 */
unsigned char *firmware_string = "Do you byte, when I knock?";

extern char __bss;		//!< the start of the uninitialized data segment
extern char __bss_end;	//!< the end of the uninitialized data segment

//! the high memory segment
extern char __text_hi, __etext_hi;

///////////////////////////////////////////////////////////////////////////////
//
// Functions
//
///////////////////////////////////////////////////////////////////////////////

extern int main(int argc, char **argv);     //!< the user main()


//! show ON string
extern inline void show_on(void) {
  cls();

#ifdef CONF_ASCII
  cputs("ON");
#else
#ifdef CONF_CONIO
  cputc_native_4(0x38);
  cputc_native_3(0x3d);
  cputc_native_2(0x7c);
  cputc_native_1(0x7e);
  cputc_native_0(0x6d);
#else
  lcd_digit(1);
#endif
#endif
#ifndef CONF_LCD_REFRESH
  lcd_refresh();
#endif
  delay(250);
}

//! show OFF string
extern inline void show_off(void) {
  cls();

#ifdef CONF_ASCII
  cputs("OFF");
#else
#ifdef CONF_CONIO
  cputc_native_4(0x7e);
  cputc_native_3(0x1d);
  cputc_native_2(0x1d);
#else
  lcd_digit(0);
#endif
#endif
#ifndef CONF_LCD_REFRESH
  lcd_refresh();
#endif
}


//! the beginning of everything
/*! initially called by ROM
 */
void kmain(void) __attribute__((noreturn));
void kmain(void)
{
  int reset_after_shutdown=0;
#ifdef CONF_DKEY
  int c;
#endif

  /* Install the text.hi segment in the correct place.  The
   * firmware loader puts it in the bss segment, we copy it 
   * to it's final location.
   */
  memcpy(&__text_hi, &__bss, &__etext_hi - &__text_hi);

  reset_vector = rom_reset_vector;

  /* Turn off motor, since writing to hitext manipulates motors */
  motor_controller = 0;
  
  memset(&__bss, 0, &__bss_end - &__bss);

#ifdef CONF_MM
  mm_init();
#endif

  while (1) {
    power_init();

#ifdef CONF_AUTOSHUTOFF
    shutoff_init();
#endif

    lcd_init();

#ifdef CONF_DSOUND
    dsound_init();
#endif
#ifdef CONF_TIME
    systime_init();
#endif
#ifdef CONF_DSENSOR
    ds_init();
#endif
#ifdef CONF_DMOTOR
    dm_init();
#endif
#ifdef CONF_LNP
    lnp_init();
    lnp_logical_init();
#endif
#ifdef CONF_TM
    tm_init();
#endif
#ifdef CONF_PROGRAM
    program_init();
#endif

    show_on();

    // wait till power key released
    //
#ifdef CONF_DKEY
    dkey_multi=KEY_ANY;
    while((c=dkey_multi) & KEY_ONOFF);
#else
    while (PRESSED(dbutton(), BUTTON_ONOFF));
    delay(100);
#endif

    cls();
#ifndef CONF_PROGRAM
    lcd_show(man_run);
#ifndef CONF_LCD_REFRESH
    lcd_refresh();
#endif
#endif

    // run app
    //
#ifdef CONF_TM
#ifndef CONF_PROGRAM
    execi(&main,0,0,PRIO_NORMAL,DEFAULT_STACK_SIZE);
#endif
    tm_start();
#else
    main(0,0);
#endif

    show_off();

    // ON/OFF + PROGRAM -> erase firmware
#ifdef CONF_DKEY
    while((c=dkey_multi) & KEY_ONOFF)
      if(c&KEY_PRGM)
        reset_after_shutdown=1;
#else
    while (PRESSED(dbutton(), BUTTON_ONOFF))
      if (PRESSED(dbutton(), BUTTON_PROGRAM))
        reset_after_shutdown=1;
#endif

#ifdef CONF_PROGRAM
    program_shutdown();
#endif
#ifdef CONF_LNP
    lnp_logical_shutdown();
#endif
#ifdef CONF_DMOTOR
    dm_shutdown();
#endif
#ifdef CONF_DSENSOR
    ds_shutdown();
#endif
#ifdef CONF_TIME
    systime_shutdown();
#endif

    if (reset_after_shutdown)
      rom_reset();

    lcd_clear();
    lcd_power_off();

    power_off();
  }
}
