/*! \file   conio.c
    \brief  Implementation: console input / output
    \author Markus L. Noga <markus@noga.de>

    \warning If CONF_LCD_REFRESH is set in config.h, the kernel will refresh
    the display automatically every 100ms.
    Otherwise, display updates are realized exclusively by
    lcd_refresh().

    \par Display positions
    Digit display positions are denumerated from right to left, starting
    with 0 for the digit right to the running man. Digit 5 is only partially
    present on the RCXs display.

    \par Native segment masks
    In these bitmasks, bit 0 toggles the middle segment. Bit 1 toggles the 
    top right segment, and the remaining segments are denumerated
    counterclockwise. The dot isn't encoded because it is desirable
    to seperate its positioning from the number display code.
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
 */

#include <conio.h>
#include <mem.h>

#ifdef CONF_CONIO

///////////////////////////////////////////////////////////////////////////////
//
// Definitions
//
///////////////////////////////////////////////////////////////////////////////

//! hex display codes
//
const char hex_display_codes[] =
{
  0x7e,				// 0
   0x42,			// 1
   0x37,			// 2
   0x67,			// 3
   0x4b,			// 4
   0x6d,			// 5
   0x7d,			// 6
   0x46,			// 7
   0x7f,			// 8
   0x6f,			// 9
   0x5f,			// A 
   0x79,			// b 
   0x31,			// c
   0x73,			// d 
   0x3d,			// E
   0x1d,			// F
};

#ifdef CONF_ASCII

//! ASCII display codes
/*! This is a 7-bit ASCII table only.
 */
const char ascii_display_codes[] =
{
  0x00, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,	// non-printables
   0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,	// -> underscore
   0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,	// except 0x00.
   0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,

  0x00,				// 32 ' '
   0x42,			// 33 ! 1
   0x0a,			// 34 "
   0x7b,			// 35 #
   0x6d,			// 36 $ 5 S Z
   0x13,			// 37 % /
   0x7d,			// 38 & 6
   0x08,			// 39 '  alt: ` 
   0x3c,			// 40 ( C [ {
   0x66,			// 41 ) ] }
   0x5b,			// 42 * H K X
   0x43,			// 43 +  alt: 0x19 (worse)
   0x10,			// 44 , .  alt: 0x40
   0x01,			// 45 -
   0x10,			// 46 . alt: 0x40
   0x13,			// 47 /

  0x7e,				// 48 0 0
   0x42,			// 49 1
   0x37,			// 50 2
   0x67,			// 51 3
   0x4b,			// 52 4
   0x6d,			// 53 5
   0x7d,			// 54 6
   0x46,			// 55 7
   0x7f,			// 56 8
   0x6f,			// 57 9

  0x21,				// 58 : ; = alt:0x5 (worse)
   0x21,			// 59 ;
   0x31,			// 60 < c alt:0xd (worse)       
   0x21,			// 61 =
   0x61,			// 62 >   alt: 0x7 (worse)
   0x17,			// 63 ?
   0x3f,			// 64 @ alt: e

  0x5f,				// 65 A
   0x79,			// 66 b 
   0x3c,			// 67 C
   0x73,			// 68 d
   0x3d,			// 69 E
   0x1d,			// 70 F
   0x7c,			// 71 G
   0x5b,			// 72 H
   0x42,			// 73 I 1
   0x62,			// 74 J
   0x5b,			// 75 K
   0x38,			// 76 L
   0x5e,			// 77 M N
   0x5e,			// 78 N
   0x7e,			// 79 O alt: o
   0x1f,			// 80 P
   0x4f,			// 81 Q 
   0x11,			// 82 r
   0x6d,			// 83 S
   0x46,			// 84 T alt: t
   0x7a,			// 85 U V W
   0x7a,			// 86 V
   0x7a,			// 87 W
   0x5b,			// 88 X
   0x6b,			// 89 Y
   0x37,			// 90 Z

  0x3c,				// 91 [
   0x49,			// 92 '\\'
   0x66,			// 93 ]
   0x0e,			// 94 ^ ~
   0x20,			// 95 _
   0x02,			// 96 ` alt: '

  0x5f,				// 97 A R
   0x79,			// 98 b 
   0x31,			// 99 c
   0x73,			// 100 d 
   0x3d,			// 101 E
   0x1d,			// 102 F
   0x7c,			// 103 G
   0x59,			// 104 h
   0x42,			// 105 I 1
   0x62,			// 106 J alt 0x60 (worse)
   0x5b,			// 107 K alt h (worse?)
   0x38,			// 108 L
   0x51,			// 109 m n
   0x51,			// 110 n
   0x71,			// 111 o
   0x1f,			// 112 P
   0x4f,			// 113 q        
   0x11,			// 114 r
   0x6d,			// 115 S
   0x39,			// 116 t
   0x70,			// 117 u v w
   0x70,			// 118 v
   0x70,			// 119 w
   0x5b,			// 120 X
   0x6b,			// 121 Y
   0x37,			// 122 Z

  0x3c,				// 123 {
   0x18,			// 124 | (left) alt: 1 (worse)
   0x66,			// 125 }
   0x0e,			// 126 ~
   0x00				// 127 "" 127 empty
};

#endif // CONF_ASCII

///////////////////////////////////////////////////////////////////////////////
//
// Functions
//
///////////////////////////////////////////////////////////////////////////////

#endif // CONF_CONIO

//! uncalibrated delay loop
/*! \param ms approximate time in ms
 */
void delay(unsigned ms)
{
  unsigned i;

  while (ms-- > 0)
    for (i = 0; i < 600; i++)	// not well calibrated.
      ;

}

#ifdef CONF_CONIO

//! display native mode segment mask
/*! \param mask the segment mask.
   \param pos  the desired display position.

   this is a dispatcher for the fixed position routines.
 */
void cputc_native(char mask, int pos)
{
  switch (pos) {
  case 0:
    cputc_native_0(mask);
    break;
  case 1:
    cputc_native_1(mask);
    break;
  case 2:
    cputc_native_2(mask);
    break;
  case 3:
    cputc_native_3(mask);
    break;
  case 4:
    cputc_native_4(mask);
    break;
  case 5:
    cputc_native_5(mask);
  }
}

//! display native mode segment mask at display position 0
/*! \param mask the mask to display
 */
void cputc_native_0(char mask)
{
  // gcc is stupid
  // doesn't re-use constant values in registers.
  // re-ordered stores to help him.

  bit_load(mask, 0x2);
  dlcd_store(LCD_0_TOP);
  bit_load(mask, 0x0);
  dlcd_store(LCD_0_MID);
  bit_load(mask, 0x5);
  dlcd_store(LCD_0_BOT);
  bit_load(mask, 0x1);
  dlcd_store(LCD_0_TOPR);
  bit_load(mask, 0x6);
  dlcd_store(LCD_0_BOTR);
  bit_load(mask, 0x3);
  dlcd_store(LCD_0_TOPL);
  bit_load(mask, 0x4);
  dlcd_store(LCD_0_BOTL);
}

//! display native mode segment mask at display position 1
/*! \param mask the mask to display
 */
void cputc_native_1(char mask)
{
  bit_load(mask, 0x2);
  dlcd_store(LCD_1_TOP);
  bit_load(mask, 0x0);
  dlcd_store(LCD_1_MID);
  bit_load(mask, 0x5);
  dlcd_store(LCD_1_BOT);
  bit_load(mask, 0x1);
  dlcd_store(LCD_1_TOPR);
  bit_load(mask, 0x6);
  dlcd_store(LCD_1_BOTR);
  bit_load(mask, 0x3);
  dlcd_store(LCD_1_TOPL);
  bit_load(mask, 0x4);
  dlcd_store(LCD_1_BOTL);
}

//! display native mode segment mask at display position 2
/*! \param mask the mask to display
 */
void cputc_native_2(char mask)
{
  bit_load(mask, 0x2);
  dlcd_store(LCD_2_TOP);
  bit_load(mask, 0x0);
  dlcd_store(LCD_2_MID);
  bit_load(mask, 0x5);
  dlcd_store(LCD_2_BOT);
  dlcd_hide(LCD_2_DOT);
  bit_load(mask, 0x1);
  dlcd_store(LCD_2_TOPR);
  bit_load(mask, 0x6);
  dlcd_store(LCD_2_BOTR);
  bit_load(mask, 0x3);
  dlcd_store(LCD_2_TOPL);
  bit_load(mask, 0x4);
  dlcd_store(LCD_2_BOTL);
}

//! display native mode segment mask at display position 3
/*! \param mask the mask to display
 */
void cputc_native_3(char mask)
{
  dlcd_hide(LCD_3_DOT);
  bit_load(mask, 0x2);
  dlcd_store(LCD_3_TOP);
  bit_load(mask, 0x0);
  dlcd_store(LCD_3_MID);
  bit_load(mask, 0x5);
  dlcd_store(LCD_3_BOT);
  bit_load(mask, 0x1);
  dlcd_store(LCD_3_TOPR);
  bit_load(mask, 0x6);
  dlcd_store(LCD_3_BOTR);
  bit_load(mask, 0x3);
  dlcd_store(LCD_3_TOPL);
  bit_load(mask, 0x4);
  dlcd_store(LCD_3_BOTL);
}

//! display native mode segment mask at display position 4
/*! \param mask the mask to display
 */
void cputc_native_4(char mask)
{
  dlcd_hide(LCD_4_DOT);
  bit_load(mask, 0x2);
  dlcd_store(LCD_4_TOP);
  bit_load(mask, 0x0);
  dlcd_store(LCD_4_MID);
  bit_load(mask, 0x5);
  dlcd_store(LCD_4_BOT);
  bit_load(mask, 0x1);
  dlcd_store(LCD_4_TOPR);
  bit_load(mask, 0x6);
  dlcd_store(LCD_4_BOTR);
  bit_load(mask, 0x3);
  dlcd_store(LCD_4_TOPL);
  bit_load(mask, 0x4);
  dlcd_store(LCD_4_BOTL);
}

//! display native mode segment mask at display position 5
/*! \param mask the mask to display. only the middle segment is 
   present on the display.
 */
void cputc_native_5(char mask)
{
  bit_load(mask, 0x0);
  dlcd_store(LCD_5_MID);
}

//! display a hexword in the four leftmost positions.
/*! \param word the hexword

   position 0 is unaffected by this call.
 */
void cputw(unsigned word)
{
  int i;

  cputc_native(0, 5);
  for (i = 1; i <= 4; i++) {
    cputc_hex(word & 0x0f, i);
    word >>= 4;
  }

#if !defined(CONF_LCD_REFRESH)
  lcd_refresh();
#endif
}

#ifdef CONF_ASCII

//! display an ASCIIZ string
/*! \param s the string

   only the first 5 characters will be displayed.
   if there are less than 5 characters, the remaining display
   positions will be cleared.
 */
void cputs(char *s)
{
  int i;

  cputc_native(0, 5);
  for (i = 4; (*s) && (i >= 0);)
    cputc(*(s++), i--);
  while (i >= 1)
    cputc_native(0, i--);

#if !defined(CONF_LCD_REFRESH)
  lcd_refresh();
#endif
}

//! clear user portion of screen
void cls() {
  cputs("    ");
}
#endif // CONF_ASCII

#endif // CONF_CONIO
