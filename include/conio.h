/*! \file   include/conio.h
    \brief  console input / output
    \author Markus L. Noga <markus@noga.de>

    \warning If CONF_LCD_REFRESH is set in config.h, the kernel will
    refresh the display automatically every 100ms.
    Otherwise, display updates are realized exclusively by
    lcd_refresh().

    \par Display positions
    Digit display positions are denumerated from right to left,
    starting with 0 for the digit right to the running man. Digit 5
    is only partially present on the RCXs display.

    \par Native segment masks
    In these bitmasks, bit 0 toggles the middle segment. Bit 1 toggles
    the top right segment, and the remaining segments are denumerated
    counterclockwise. The dot isn't encoded because it is desirable
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

#ifndef __conio_h_
#define __conio_h_

#ifdef  __cplusplus
extern "C" {
#endif

#include <config.h>

#ifdef CONF_CONIO

#include <rom/lcd.h>
#include <dlcd.h>
#include <dkey.h>

///////////////////////////////////////////////////////////////////////
//
// Definitions
//
///////////////////////////////////////////////////////////////////////

//
// hex display codes
//
extern const char hex_display_codes[];

#ifdef CONF_ASCII

//
// ascii display codes
// only lower 128 bit, please!
//
extern const char ascii_display_codes[];

#endif // CONF_ASCII

///////////////////////////////////////////////////////////////////////
//
// Functions
//
///////////////////////////////////////////////////////////////////////

#endif // CONF_CONIO

//
// delay by approximately d ms
//
extern void delay(unsigned d);

#ifdef CONF_CONIO

//
// display native mode segment mask at fixed display position
//
// encoding: middle=1, topr=2, top=4, ... (counterclockwise)
// dot not included because not reliably present.
//
extern void cputc_native_0(char mask);
extern void cputc_native_1(char mask);
extern void cputc_native_2(char mask);
extern void cputc_native_3(char mask);
extern void cputc_native_4(char mask);
extern void cputc_native_5(char mask);

// a dispatcher for the fixed position versions
extern void cputc_native(char mask, int pos);

extern inline void cputc_hex_0(unsigned nibble)
{
  cputc_native_0(hex_display_codes[(nibble) & 0x0f]);
}
extern inline void cputc_hex_1(unsigned nibble)
{
  cputc_native_1(hex_display_codes[(nibble) & 0x0f]);
}
extern inline void cputc_hex_2(unsigned nibble)
{
  cputc_native_2(hex_display_codes[(nibble) & 0x0f]);
}
extern inline void cputc_hex_3(unsigned nibble)
{
  cputc_native_3(hex_display_codes[(nibble) & 0x0f]);
}
extern inline void cputc_hex_4(unsigned nibble)
{
  cputc_native_4(hex_display_codes[(nibble) & 0x0f]);
}
extern inline void cputc_hex_5(unsigned nibble)
{
  cputc_native_5(hex_display_codes[(nibble) & 0x0f]);
}

//! a dispatcher for the fixed position versions
extern inline void cputc_hex(char c, int pos)
{
  cputc_native(hex_display_codes[(c) & 0x7f], pos);
}

//! display a hexword
//
extern void cputw(unsigned word);

#ifdef CONF_ASCII
extern inline void cputc_0(unsigned c)
{
  cputc_native_0(ascii_display_codes[(c) & 0x7f]);
}
extern inline void cputc_1(unsigned c)
{
  cputc_native_1(ascii_display_codes[(c) & 0x7f]);
}
extern inline void cputc_2(unsigned c)
{
  cputc_native_2(ascii_display_codes[(c) & 0x7f]);
}
extern inline void cputc_3(unsigned c)
{
  cputc_native_3(ascii_display_codes[(c) & 0x7f]);
}
extern inline void cputc_4(unsigned c)
{
  cputc_native_4(ascii_display_codes[(c) & 0x7f]);
}
extern inline void cputc_5(unsigned c)
{
  cputc_native_5(ascii_display_codes[(c) & 0x7f]);
}

//! display ascii character c at display position pos
//
extern inline void cputc(char c, int pos)
{
  cputc_native(ascii_display_codes[(c) & 0x7f], pos);
}

//
// display a zero terminated string s
// max first 5 characters
//
extern void cputs(char *s);

//! clear user portion of screen
extern void cls();

#else

#define cls() lcd_clear()

#endif // CONF_ASCII

#else

#define cls() lcd_clear()

#endif // CONF_CONIO

#ifdef  __cplusplus
}
#endif

#endif // __conio_h__
