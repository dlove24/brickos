/*! \file   include/conio.h
    \brief  Interface: console input / output
    \author Markus L. Noga <markus@noga.de>

    \warning If CONF_LCD_REFRESH is set in config.h, the kernel will
    refresh the display automatically every 100ms.
    Otherwise, display updates are realized exclusively by
    lcd_refresh().

    \par Display positions
    Digit display positions are denumerated from right to left,
    starting with 0 for the digit to the right of the running man. 
	
	\par
    LCD Postions:  5 4 3 2 1 {man} 0

	\par
    NOTE: Position 5 is only partially present on the LCD display.

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

#include <sys/lcd.h>
#include <dlcd.h>
#include <dkey.h>

///////////////////////////////////////////////////////////////////////
//
// Definitions
//
///////////////////////////////////////////////////////////////////////

#ifndef DOXYGEN_SHOULD_SKIP_INTERNALS

//! Table: list of native patterns, one for each HEX character
/*! \index HEX char value (0-9, a-f)
*/
extern const char hex_display_codes[];

#ifdef CONF_ASCII

//! Table: list of native patterns, one for each ASCII character
/*! \index ASCII char value (least significant 7 bits ONLY)
*/
extern const char ascii_display_codes[];

#endif // CONF_ASCII

#endif // DOXYGEN_SHOULD_SKIP_INTERNALS

///////////////////////////////////////////////////////////////////////
//
// Functions
//
///////////////////////////////////////////////////////////////////////

#endif // CONF_CONIO

//! delay approximately ms mSec
/*! \todo why is delay() in this file?
*/
extern void delay(unsigned ms);

#ifdef CONF_CONIO

//
// display native mode segment mask at fixed display position
//
// encoding: middle=1, topr=2, top=4, ... (counterclockwise)
// dot not included because not reliably present.
//
//! write bit-pattern for segments at position 0 of LCD
extern void cputc_native_0(char mask);
//! write bit-pattern for segments at position 1 of LCD
extern void cputc_native_1(char mask);
//! write bit-pattern for segments at position 2 of LCD
extern void cputc_native_2(char mask);
//! write bit-pattern for segments at position 3 of LCD
extern void cputc_native_3(char mask);
//! write bit-pattern for segments at position 4 of LCD
extern void cputc_native_4(char mask);
//! write bit-pattern for segments at position 5 of LCD
extern void cputc_native_5(char mask);

//! Set/Clear individual segments at specified position of LCD
/*! (this is essentially a dispatcher for cputc_native_[0-5] functions)
 *  \param mask the segment pattern to be displayed
 *  \param pos the location at which to display the segment pattern
 *  \return Nothing
 */
extern void cputc_native(char mask, int pos);

//! write HEX digit to position 0 of LCD
extern inline void cputc_hex_0(unsigned nibble)
{
  cputc_native_0(hex_display_codes[(nibble) & 0x0f]);
}
//! write HEX digit to position 1 of LCD
extern inline void cputc_hex_1(unsigned nibble)
{
  cputc_native_1(hex_display_codes[(nibble) & 0x0f]);
}
//! write HEX digit to position 2 of LCD
extern inline void cputc_hex_2(unsigned nibble)
{
  cputc_native_2(hex_display_codes[(nibble) & 0x0f]);
}
//! write HEX digit to position 3 of LCD
extern inline void cputc_hex_3(unsigned nibble)
{
  cputc_native_3(hex_display_codes[(nibble) & 0x0f]);
}
//! write HEX digit to position 4 of LCD
extern inline void cputc_hex_4(unsigned nibble)
{
  cputc_native_4(hex_display_codes[(nibble) & 0x0f]);
}
//! write HEX digit to position 5 of LCD
extern inline void cputc_hex_5(unsigned nibble)
{
  cputc_native_5(hex_display_codes[(nibble) & 0x0f]);
}

//! Write HEX digit to specified position of LCD
/*! (this is essentially a dispatcher for cputc_hex_[0-5] functions)
 *  \param c the HEX digit to be displayed
 *  \param pos the location at which to display the HEX digit
 *  \return Nothing
 */
extern inline void cputc_hex(char c, int pos)
{
  cputc_native(hex_display_codes[(c) & 0x7f], pos);
}

//! Write a HEX word to LCD
extern void cputw(unsigned word);

#ifdef CONF_ASCII
//! write ASCII char to position 0 of LCD
extern inline void cputc_0(unsigned c)
{
  cputc_native_0(ascii_display_codes[(c) & 0x7f]);
}
//! write ASCII char to position 1 of LCD
extern inline void cputc_1(unsigned c)
{
  cputc_native_1(ascii_display_codes[(c) & 0x7f]);
}
//! write ASCII char to position 2 of LCD
extern inline void cputc_2(unsigned c)
{
  cputc_native_2(ascii_display_codes[(c) & 0x7f]);
}
//! write ASCII char to position 3 of LCD
extern inline void cputc_3(unsigned c)
{
  cputc_native_3(ascii_display_codes[(c) & 0x7f]);
}
//! write ASCII char to position 4 of LCD
extern inline void cputc_4(unsigned c)
{
  cputc_native_4(ascii_display_codes[(c) & 0x7f]);
}
//! write ASCII char to position 5 of LCD
extern inline void cputc_5(unsigned c)
{
  cputc_native_5(ascii_display_codes[(c) & 0x7f]);
}

//! Write ASCII character to specified position of LCD
/*! (this is essentially a dispatcher for cputc_[0-5] functions)
 *  \param c the ASCII char to be displayed
 *  \param pos the location at which to display the ASCII char
 *  \return Nothing
 */
extern inline void cputc(char c, int pos)
{
  cputc_native(ascii_display_codes[(c) & 0x7f], pos);
}

//
//! Write string s to LCD (Only first 5 chars)
//
extern void cputs(char *s);

//
//! clear user portion of LCD
//
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
