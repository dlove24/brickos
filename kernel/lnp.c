/*! \file   lnp.c
    \brief  legOS networking protocol
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

/*
 *  2000.05.01 - Paolo Masetti <paolo.masetti@itlug.org>
 *
 *	- IR lcd now reflect IR mode (near/far)
 *
 */

#include <sys/lnp.h>

#ifdef CONF_LNP

#include <sys/lnp-logical.h>
#include <sys/irq.h>

#ifdef CONF_VIS
#include <dlcd.h>
#include <sys/lcd.h>
#endif

#ifdef CONF_AUTOSHUTOFF
#include <sys/timeout.h>
#endif

#include <string.h>

///////////////////////////////////////////////////////////////////////////////
//
// Variables
//
///////////////////////////////////////////////////////////////////////////////

#ifndef CONF_HOST
#ifdef CONF_TM

#include <semaphore.h>

//!< transmitter access semaphore
extern sem_t tx_sem;

#endif
#endif

//! the timeout counter in ms
volatile unsigned short lnp_timeout_counter;

//! the timeout length in ms
unsigned short lnp_timeout=LNP_BYTE_TIMEOUT;

//! the integrity layer state
lnp_integrity_state_t lnp_integrity_state;

//! there are no ports for integrity layer packets, so just one handler.
/*! FIXME: uninitialized
*/
volatile lnp_integrity_handler_t lnp_integrity_handler;

//! addressing layer packets may be directed to a variety of ports.
/*! FIXME: uninitialized
    FIXME: inefficient if LNP_PORT_MASK doesn't adhere to 0...01...1 scheme.
*/
volatile lnp_addressing_handler_t lnp_addressing_handler[LNP_PORTMASK+1];

//! the LNP transmit buffer
static unsigned char lnp_buffer[256+3];


///////////////////////////////////////////////////////////////////////////////
//
// Functions
//
///////////////////////////////////////////////////////////////////////////////

#ifdef CONF_HOST
unsigned short lnp_checksum(
			 const unsigned char *data,
			 unsigned length
)
{
  unsigned char a = 0xff;
  unsigned char b = 0xff;

  while (length > 0) {
    a = a + *data;
    b = b + a;
    data++;
    length--;
  }

  return a + (b << 8);
}
#else
__asm__("
.text
_lnp_checksum:
    ; r0 data, r1 length;

    add.w r0,r1     ; r1 end

    mov.b #0xff,r2l ; r2l a
    mov.b #0xff,r2h ; r2h b

  lnp_keepsumming:
    mov.b @r0+,r3l  ; r3l = *(data++)
    add.b r3l,r2l
    add.b r2l,r2h
    cmp.w r0,r1
    bne lnp_keepsumming

    mov.w r2,r0
    rts
  ");
#endif

//! send a LNP integrity layer packet of given length
/*! \return 0 on success.
*/
int lnp_integrity_write(const unsigned char *data,unsigned char length) {

#ifndef CONF_HOST
#ifdef CONF_TM
  sem_wait(&tx_sem);
#endif
#endif

  lnp_buffer[0]=0xf0;
  lnp_buffer[1]=length;
  memcpy(lnp_buffer+2,data,length);
  lnp_buffer[length+2]=(unsigned char) lnp_checksum(lnp_buffer,length+2);

  return lnp_logical_write(lnp_buffer,length+3);
}

//! send a LNP addressing layer packet of given length
/*! \return 0 on success.
*/
int lnp_addressing_write(const unsigned char *data,unsigned char length,
                         unsigned char dest,unsigned char srcport) {

#ifndef CONF_HOST
#ifdef CONF_TM
  sem_wait(&tx_sem);
#endif
#endif

  lnp_buffer[0]=0xf1;
  lnp_buffer[1]=length+2;
  lnp_buffer[2]=dest;
  lnp_buffer[3]=LNP_HOSTADDR | (srcport & LNP_PORTMASK);
  memcpy(lnp_buffer+4,data,length);
  lnp_buffer[length+4]=(unsigned char) lnp_checksum(lnp_buffer,length+4);

  return lnp_logical_write(lnp_buffer,length+5);
}

//! handle LNP packet from the integrity layer
void lnp_receive_packet(const unsigned char *data) {
  unsigned char header=*(data++);
  unsigned char length=*(data++);

  // only handle non-degenerate packets in boot protocol 0xf0
  //
  switch(header) {
    case 0xf0:	      // raw integrity layer packet, no addressing.
      if(lnp_integrity_handler) {
#ifdef CONF_AUTOSHUTOFF
	shutoff_restart();
#endif
        lnp_integrity_handler(data,length);
      }
      break;

    case 0xf1:	      // addressing layer.
      if(length>2) {
	unsigned char dest=*(data++);

	if(LNP_HOSTADDR == (dest & LNP_HOSTMASK)) {
	  unsigned char port=dest & LNP_PORTMASK;

	  if(lnp_addressing_handler[port]) {
   	    unsigned char src=*(data++);
#ifdef CONF_AUTOSHUTOFF
	    shutoff_restart();
#endif
	    lnp_addressing_handler[port](data,length-2,src);
	  }
	}
      }

  } // switch(header)
}

//! receive a byte, decoding LNP packets with a state machine.
void lnp_integrity_byte(unsigned char b) {
  static unsigned char buffer[256+3];
  static int bytesRead,endOfData;

  if(lnp_integrity_state==LNPwaitHeader)
    bytesRead=0;

  buffer[bytesRead++]=b;

  switch(lnp_integrity_state) {
    case LNPwaitHeader:
      // valid headers are 0xf0 .. 0xf7
      //
      if((b & (unsigned char) 0xf8) == (unsigned char) 0xf0) {
#ifdef CONF_VIS
	if (lnp_logical_range_is_far()) {
          dlcd_show(LCD_IR_UPPER);
	  dlcd_show(LCD_IR_LOWER);
	} else {
	  dlcd_hide(LCD_IR_UPPER);
	  dlcd_show(LCD_IR_LOWER);
        }
#ifndef CONF_LCD_REFRESH
	lcd_refresh();
#endif
#endif
        lnp_integrity_state++;
      }
      break;

    case LNPwaitLength:
      endOfData=b+2;
      lnp_integrity_state++;
      break;

    case LNPwaitData:
      if(bytesRead==endOfData)
	lnp_integrity_state++;
      break;

    case LNPwaitCRC:
      if(b==(unsigned char) lnp_checksum(buffer,endOfData))
	lnp_receive_packet(buffer);
      lnp_integrity_reset();
  }
}

//! reset the integrity layer on error or timeout.
#if defined(CONF_RCX_COMPILER) || defined(CONF_HOST)
void lnp_integrity_reset(void) {
#else
HANDLER_WRAPPER("lnp_integrity_reset","lnp_integrity_reset_core");
void lnp_integrity_reset_core(void) {
#endif
#ifndef CONF_HOST
  if(tx_state>TX_IDLE) {
    txend_handler();
    tx_state=TX_COLL;
  } else
#endif
  if(lnp_integrity_state!=LNPwaitHeader) {
    lnp_integrity_state=LNPwaitHeader;

#ifdef CONF_VIS
    dlcd_hide(LCD_IR_LOWER);
    dlcd_hide(LCD_IR_UPPER);
#ifndef CONF_LCD_REFRESH
    lcd_refresh();
#endif
#endif
  }
}

//! return whether a packet is currently being received
/*! \return 1 if yes, else zero
*/
int lnp_integrity_active(void) {
  return lnp_integrity_state!=LNPwaitHeader;
}

//! reset the inter-byte timeout counter.
#if defined(CONF_RCX_COMPILER) || defined(CONF_HOST)
void lnp_timeout_reset(void) {
#else
HANDLER_WRAPPER("lnp_timeout_reset","lnp_timeout_reset_core");
void lnp_timeout_reset_core(void) {
#endif
  lnp_timeout_counter=lnp_timeout;
}

//! set the inter-byte timeout and reset the timeout counter to that value.
/*! \param  timeout the new timeout value
*/
void lnp_timeout_set(unsigned short timeout) {
  lnp_timeout_counter=lnp_timeout=timeout;
}

//! Initialise protocol handlers
/*! Adressing port 0 is reserved for the legOS program handler.
*/
void lnp_init(void) {
  int k;
  for(k=1;k<=LNP_PORTMASK;k++)
    lnp_addressing_handler[k]=LNP_DUMMY_ADDRESSING;
  lnp_integrity_handler=LNP_DUMMY_INTEGRITY;
}

#endif // CONF_LNP
