/*! \file   lcd.c
    \brief  Implementation: wrapper for ROM LCD number display functions
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
 */

#include <config.h>
#include <dlcd.h>
#include <conio.h>
#include <string.h>
#include <sys/h8.h>
#include <sys/lcd.h>
#include <rom/registers.h>
#include <lnp/sys/irq.h>

///////////////////////////////////////////////////////////////////////////////
//
// Variables
//
///////////////////////////////////////////////////////////////////////////////

#ifdef CONF_LCD_REFRESH
unsigned char lcd_refresh_counter;    //!< counter for lcd refresh in ms
unsigned char lcd_byte_counter;       //!< LCD byte to refresh
unsigned char lcd_refresh_period = 2; //!< LCD refresh period in ms
#endif

//! lcd_shadow buffer:
/*!
    contains the last display_memory bytes written to the LCD
    controller at position LCD_DATA_OFFSET.

    lcd_shadow also contains the buffer for the i2c commands
    written to the LCD controller.

    At offset LCD_SHORT_CMD a small piece of memory is reserved
    for direct commands to the LCD controller. This is used for
    power on/off, and for single byte updates.

    At offset LCD_LONG_CMD a small piece of memory is reserved
    for the i2c command header to do a full LCD update. The display
    data directly follows this header.

    The entire buffer in memory looks like this:

     0      1      2      3      4      5
    +------+------+------+------+------+---                  ---+
    | Addr | Cmd  | Data | Addr | 0x00 | display data (9 bytes) |
    +------+------+------+------+------+---                  ---+
     \__________________/ \____________________________________/
        LCD_SHORT_CMD         LCD_LONG_CMD with display data
*/
static unsigned char lcd_shadow[LCD_DATA_OFFSET + LCD_DATA_SIZE];

///////////////////////////////////////////////////////////////////////////////
//
// Functions
//
///////////////////////////////////////////////////////////////////////////////

//! show number on LCD display
/*! \param i the number
    \param n a number style
    \param c a comma style
*/
void lcd_number(int i,lcd_number_style n,lcd_comma_style c  );
#ifndef DOXYGEN_SHOULD_SKIP_THIS
__asm__("
.text
.align 1
.globl _lcd_number
_lcd_number:
        push r6         ; save r6
    
        push r2         ; comma_style  -> stack
        push r0         ; number       -> stack
    
        mov.w r1,r6     ; number_style -> r6
    
        jsr @lcd_number ; call ROM
    
        adds #0x02,sp   ; clear stack
        adds #0x02,sp
        
        pop r6          ; restore r6
        rts
        
       ");
#endif // DOXYGEN_SHOULD_SKIP_THIS

//! set single bit convenience macro
#define set(b)  __asm__ __volatile__("bset %0,@0xbb:8" : : "i"(b));
//! clear single bit convenience macro
#define clr(b)  __asm__ __volatile__("bclr %0,@0xbb:8" : : "i"(b));

//! generate the necessary delay for the i2c bus.
/*!
    the h8/300 at 16 MHz is slow enough to run without delay,
    when running this code from RAM. There might be some delay
    necessary when running from ROM.
*/
#define slowdown()

//! generate an i2c start condition.
static __inline__ void i2c_start(void)
{
    set(SDA);
    set(SCL);
    slowdown();
    clr(SDA);
    slowdown();
    clr(SCL);
    slowdown();
}

//! generate an i2c stop condition.
static __inline__ void i2c_stop(void)
{
    clr(SDA);
    set(SCL);
    slowdown();
    set(SDA);
    slowdown();
    clr(SCL);
    slowdown();
    set(SCL);		// relax output driver (saves 0.5 mA)
}

//! read the acknoledge from the i2c bus.
/*!
    Warning: the value of the acknoledge is ignored.
             We can't do much on errors anyway.
*/
static __inline__ void i2c_read_ack(void)
{
    rom_port6_ddr &= ~(1 << SDA);
    PORT6_DDR      = rom_port6_ddr;
    slowdown();
    set(SCL);
    slowdown();
    clr(SCL);
    rom_port6_ddr |= (1 << SDA);
    PORT6_DDR      = rom_port6_ddr;
    slowdown();
}

//! write one byte to the i2c bus.
/*! \param val  byte to write to the i2c bus
*/
static __inline__ void i2c_write(unsigned char val)
{
    unsigned char bit;

    for (bit = (1 << 7); bit; bit >>= 1) {
        if (bit & val) {
            set(SDA);
        } else {
            clr(SDA);
        }
        slowdown();
        set(SCL);
        slowdown();
        clr(SCL);
        slowdown();
    }
}

//! write an array of bytes to the i2c bus.
/*! \param data  array of bytes to write to the i2c bus
    \param len   number of bytes to write

    len bytes are written to the i2c bus, including initial
    start condition and final stop condition. The first byte
    must contain a device address and the r/w flag.
*/
static void lcd_write_data(unsigned char *data, unsigned char len)
{
    unsigned char i;

    i2c_start();
    for (i = 0; i < len; i++) {
        i2c_write(*data++);
        i2c_read_ack();
    }
    i2c_stop();
}


#ifdef CONF_LCD_REFRESH

//! lcd refresh handler, called from system timer interrupt
/*! refresh one byte of display_memory.

    Bytes are refreshed in round robin fashon and the lcd_shadow
    buffer is updated. If there was no difference between
    display_memory and the lcd_shadow buffer, the actual write to
    the LCD controller is skipped.

    This routine is called every 6ms from the timer interrupt,
    resulting in a complete LCD update every 54ms, which is a
    refresh rate of 18 updates per second.
*/
#ifdef CONF_RCX_COMPILER
void lcd_refresh_next_byte(void)
#else
HANDLER_WRAPPER("lcd_refresh_next_byte", "lcd_refresh_next_byte_core");
//! alternate name for the refresh next byte routine
/*! \todo find a better description for this
*/
void lcd_refresh_next_byte_core(void)
#endif
{
    unsigned char byte;

    byte = lcd_byte_counter++;
    if (lcd_byte_counter >= LCD_DATA_SIZE)
        lcd_byte_counter = 0;

    if (lcd_shadow[byte + LCD_DATA_OFFSET] == display_memory[byte])
        return;

    lcd_shadow[byte + LCD_DATA_OFFSET] = display_memory[byte];
    lcd_shadow[LCD_SHORT_CMD + 1] = byte << 1;
    lcd_shadow[LCD_SHORT_CMD + 2] = display_memory[byte];
    lcd_write_data(&lcd_shadow[LCD_SHORT_CMD], 3);
}

#endif // CONF_LCD_REFRESH

//! refresh the entire LCD display
/*! the entire 9 bytes of display_memory are written to the
    LCD controller unconditionally. lcd_shadow is updated to
    the new values.
*/
void lcd_refresh(void)
{
    unsigned char i;

    for (i = 0; i < LCD_DATA_SIZE; i++)
        lcd_shadow[i + LCD_DATA_OFFSET] = display_memory[i];
    lcd_write_data(&lcd_shadow[LCD_LONG_CMD], 11);
}

//! power on the LCD controller
/*! the LCD controller is enabled.
*/
void lcd_power_on(void)
{
    lcd_shadow[LCD_SHORT_CMD + 1] = LCD_MODE_SET | LCD_ENABLE;
    lcd_write_data(&lcd_shadow[LCD_SHORT_CMD], 2);
}

//! power off the LCD controller
/*! the LCD controller is put in low power mode and
    output drivers to the LCD controller are disabled.

    Note: without disabling the output drivers the CPU
          will not save power in sleep mode.
*/
void lcd_power_off(void)
{
    lcd_refresh();

    lcd_shadow[LCD_SHORT_CMD + 1] = LCD_MODE_SET | LCD_DISABLE;
    lcd_write_data(&lcd_shadow[LCD_SHORT_CMD], 2);

    clr(SCL);
    clr(SDA);
}

//! initialize the LCD display driver
/*! output drivers are configured as outputs.
    the lcd_shadow buffer is cleared and initialized.
    the LCD controller is enabled.
*/
void lcd_init(void)
{
    rom_port6_ddr |= (1 << SCL);
    PORT6_DDR      = rom_port6_ddr;
    clr(SCL);
    rom_port6_ddr |= (1 << SDA);
    PORT6_DDR      = rom_port6_ddr;
    clr(SDA);

    memset(lcd_shadow, 0, sizeof(lcd_shadow));
    lcd_shadow[LCD_SHORT_CMD] = LCD_DEV_ID | I2C_WRITE;
    lcd_shadow[LCD_LONG_CMD]  = LCD_DEV_ID | I2C_WRITE;

    lcd_power_on();

#ifdef CONF_LCD_REFRESH
    lcd_refresh_counter = 0;
    lcd_byte_counter = 0;
#endif
}
