/*! \file   lnp.c
    \brief  Implementation: link networking protocol
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
 *  - IR lcd now reflect IR mode (near/far)
 *
 *  2001.09.10 - Zhengrong Zang <mikezang@iname.com>
 *
 *  - Remote control buttons
 *  - Standard firmware async message
 *
 *  2002.04.23 - Ted Hess <thess@kitschensync.net>
 *
 *  - Integrate Ross Crawford/Zhengrong Zang remote control message parser
 *    and RCX op-code dispatcher
 *
 */

#include <sys/lnp.h>

#ifdef CONF_LNP

#include <sys/lnp-logical.h>
#include <sys/irq.h>
#include <stdlib.h>

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

//! LNP host address (may be changed by dll utility --node=a option)
//! Default vaule in config.h may be overidden at compile type
unsigned char lnp_hostaddr = (CONF_LNP_HOSTADDR << 4) & CONF_LNP_HOSTMASK;

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

#if !defined(CONF_MM)
static char lnp_buffer[260];
#endif // CONF_MM

#if defined(CONF_RCX_PROTOCOL)
//! remote handler
lnp_remote_handler_t lnp_remote_handler;

//! RCX protocol header bytes excluding first byte
const unsigned char lnp_rcx_header[LNP_RCX_HEADER_LENGTH] = {0xff,0x00};

//! remote header bytes excluding first 3 bytes
const unsigned char lnp_rcx_remote_op[LNP_RCX_REMOTE_OP_LENGTH] = {0xd2,0x2d};


//! temp cells for RCX firmware protocol assembly
unsigned char lnp_rcx_temp0;
unsigned char lnp_rcx_temp1;

//! checksum from RCX protocol
unsigned char lnp_rcx_checksum;

#endif

#if defined(CONF_RCX_MESSAGE)
//! message header bytes excluding first 3 bytes
const unsigned char lnp_rcx_msg_op[LNP_RCX_MSG_OP_LENGTH] = {0xf7,0x08};

//! message from standard firmware
unsigned char lnp_rcx_message;

#endif

///////////////////////////////////////////////////////////////////////////////
//
// Functions
//
///////////////////////////////////////////////////////////////////////////////

#define lnp_checksum_init(sum)  (unsigned char)((sum) = 0xff)
#define lnp_checksum_step(sum,d)  (unsigned char)((sum) += (d))

#ifdef CONF_HOST
unsigned char lnp_checksum_copy( unsigned char *dest,
                                 const unsigned char *data, 
                                 unsigned length )
{
  unsigned char a = 0xff;
  unsigned char t;

  do {
    t = *data++;
    a += t;
    *dest++ = t;
    length--;
  } while (length > 0);

  return a;
}
#else
__asm__("
.text
_lnp_checksum_copy:
    ; r0: dest, r1: data, r2: length;

    add.w r0,r2   ; r2: end
    mov.b #0xff,r3l ; r3l: a

0:
    mov.b @r1+,r3h  ; r3h = *data++
    add.b r3h,r3l ; a += r3h
    mov.b r3h,@r0 ; *dest++ = r3h
    adds  #1,r0
    cmp.w r0,r2
    bne   0b

    sub.w r0,r0
    mov.b r3l,r0l
    rts
  ");
#endif

//! send a LNP integrity layer packet of given length
/*! \return 0 on success.
*/
int lnp_integrity_write(const unsigned char *data,unsigned char length) {
  int r;
#if defined(CONF_MM)
  char* buffer_ptr = malloc(length+3);
#else // CONF_MM
  char* buffer_ptr = lnp_buffer;
#endif // CONF_MM
  unsigned char c = lnp_checksum_copy( buffer_ptr+2, data, length);
  lnp_checksum_step( c, buffer_ptr[0]=0xf0 );
  lnp_checksum_step( c, buffer_ptr[1]=length );
  buffer_ptr[length+2] = c;
  r = lnp_logical_write(buffer_ptr,length+3);
#if defined(CONF_MM)
  free(buffer_ptr); 
#endif // CONF_MM
  return r;
}

//! send a LNP addressing layer packet of given length
/*! \return 0 on success.
*/
int lnp_addressing_write(const unsigned char *data,unsigned char length,
                         unsigned char dest,unsigned char srcport) {
  int r;
#if defined(CONF_MM)
  char* buffer_ptr = malloc(length+5);
#else // CONF_MM
  char* buffer_ptr = lnp_buffer;
#endif // CONF_MM
  unsigned char c = lnp_checksum_copy( buffer_ptr+4, data, length );
  lnp_checksum_step( c, buffer_ptr[0]=0xf1 );
  lnp_checksum_step( c, buffer_ptr[1]=length+2 );
  lnp_checksum_step( c, buffer_ptr[2]=dest );
  lnp_checksum_step( c, buffer_ptr[3]=
                   (lnp_hostaddr | (srcport & LNP_PORTMASK)) );
  buffer_ptr[length+4] = c;
  r = lnp_logical_write(buffer_ptr,length+5);
#if defined(CONF_MM)
  free(buffer_ptr); 
#endif // CONF_MM
  return r;
}

//! handle LNP packet from the integrity layer
void lnp_receive_packet(const unsigned char *data) {
  unsigned char header=*(data++);
  unsigned char length=*(data++);
  lnp_integrity_handler_t intgh;
  lnp_addressing_handler_t addrh;

  // only handle non-degenerate packets in boot protocol 0xf0
  //
  switch(header) {
    case 0xf0:        // raw integrity layer packet, no addressing.
      intgh = lnp_integrity_handler;
      if(intgh) {
#ifdef CONF_AUTOSHUTOFF
        shutoff_restart();
#endif
        intgh(data,length);
      }
      break;

    case 0xf1:        // addressing layer.
      if(length>2) {
        unsigned char dest=*(data++);

        if(lnp_hostaddr == (dest & LNP_HOSTMASK)) {
          unsigned char port=dest & LNP_PORTMASK;
          addrh = lnp_addressing_handler[port];
          if(addrh) {
            unsigned char src=*(data++);
#ifdef CONF_AUTOSHUTOFF
            shutoff_restart();
#endif
            addrh(data,length-2,src);
          }
        }
      }

  } // switch(header)
}

//! receive a byte, decoding LNP packets with a state machine.
void lnp_integrity_byte(unsigned char b) {
  static unsigned char buffer[256+3];
  static int bytesRead,endOfData;
  static unsigned char chk;

  if(lnp_integrity_state==LNPwaitHeader)
    bytesRead=0;

  buffer[bytesRead++]=b;

  switch(lnp_integrity_state) {
    case LNPwaitHeader:
      // valid headers are 0xf0 .. 0xf7
      //
      if(((b & 0xf8) == 0xf0) || (b == 0x55)) {
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
        // Init checksum
        lnp_checksum_init( chk );
    
        // switch on protocol header
        if (b == 0x55) {
#if defined(CONF_RCX_PROTOCOL) || defined(CONF_RCX_MESSAGE)
          // 0x55 is header for standard firmware message
          lnp_integrity_state = LNPwaitRMH1;
#else
          lnp_integrity_reset();
#endif
        } else {
          lnp_integrity_state++;
        }
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
      if(b==chk)
  lnp_receive_packet(buffer);
      lnp_integrity_reset();
    break;

#if defined(CONF_RCX_PROTOCOL) || defined (CONF_RCX_MESSAGE)
  // state machine to handle remote
    case LNPwaitRMH1:
    case LNPwaitRMH2:
      // waiting for header bytes
      if ( b == lnp_rcx_header[ lnp_integrity_state-LNPwaitRMH1 ] )
        lnp_integrity_state++;
      else
        lnp_integrity_reset();
      break;

    case LNPwaitRMH3:
    case LNPwaitRMH4:
      if ( b == lnp_rcx_remote_op[ lnp_integrity_state-LNPwaitRMH3 ] )
        lnp_integrity_state++;
#if defined(CONF_RCX_MESSAGE)
      else if ( b == lnp_rcx_msg_op[ lnp_integrity_state-LNPwaitRMH3 ] )
        lnp_integrity_state = LNPwaitMH4;
#endif
      else
        lnp_integrity_reset();
      break;

    case LNPwaitRB0:
      lnp_rcx_temp0 = b;
      lnp_integrity_state++;
      break;

    case LNPwaitRB0I:
      if ( (unsigned char)~b == lnp_rcx_temp0 )
        lnp_integrity_state++;
      else
        lnp_integrity_reset();
      break;

    case LNPwaitRB1:
      lnp_rcx_temp1 = b;
      lnp_integrity_state++;
      break;

    case LNPwaitRB1I:
      if ( (unsigned char)~b == lnp_rcx_temp1 )
        lnp_integrity_state++;
      else
        lnp_integrity_reset();
      break;

    case LNPwaitRC:
      lnp_rcx_checksum = 0xd2 + lnp_rcx_temp0 + lnp_rcx_temp1;
      if ( b == lnp_rcx_checksum )
        lnp_integrity_state++;
      else
        lnp_integrity_reset();
      break;

    case LNPwaitRCI:
      // if checksum valid and remote handler has been installed, call remote handler
    if ( b == (unsigned char)~lnp_rcx_checksum) {
#if defined(CONF_RCX_MESSAGE)
     // if a message, set message number and exit
     if (lnp_rcx_temp1 & 0x07)
     {
        lnp_rcx_message = (lnp_rcx_temp1 > 2) ? 3 : lnp_rcx_temp1;
     } 
     else
#endif
     {
        // Invoke remote handler if any
        lnp_remote_handler_t rmth = lnp_remote_handler;
        if (rmth)
          rmth( (lnp_rcx_temp0<<8)+lnp_rcx_temp1 );
        }
      }
      // reset state machine when done
      lnp_integrity_reset();
      break;
#endif

#if defined(CONF_RCX_MESSAGE)
    // state machine to handle RCX protocol messages
    case LNPwaitMH3:
    case LNPwaitMH4:
      if ( b == lnp_rcx_msg_op[ lnp_integrity_state-LNPwaitMH3 ] )
        lnp_integrity_state++;
      else
        lnp_integrity_reset();
      break;

    case LNPwaitMN:
      lnp_rcx_temp0 = b;
      lnp_integrity_state++;
      break;

    case LNPwaitMNC:
      if ( (unsigned char)~b == lnp_rcx_temp0 )
        lnp_integrity_state++;
      else
        lnp_integrity_reset();
      break;

    case LNPwaitMC:
      lnp_rcx_temp1 = 0xf7 + lnp_rcx_temp0;

      if (b == lnp_rcx_temp1)
         lnp_integrity_state++;
      else
        lnp_integrity_reset();
      break;

    case LNPwaitMCC:
      // set message variable if it is valid message
      if ( (unsigned char)~b == lnp_rcx_temp1 )
        lnp_rcx_message = lnp_rcx_temp0;
      // reset state machine
      lnp_integrity_reset();
      break;
#endif
  }
  // Accumulate checksum
  lnp_checksum_step( chk, b );
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
/*! Adressing port 0 is reserved for the program handler.
*/
void lnp_init(void) {
  int k;
  
  for(k=1; k<=LNP_PORTMASK; k++)
    lnp_addressing_handler[k]=LNP_DUMMY_ADDRESSING;
  lnp_integrity_handler=LNP_DUMMY_INTEGRITY;

#if defined(CONF_RCX_PROTOCOL)
  lnp_remote_handler=LNP_DUMMY_REMOTE;
#endif
#if defined(CONF_RCX_MESSAGE)
  clear_msg();
#endif
}

#ifdef CONF_RCX_MESSAGE
wakeup_t msg_received(wakeup_t m) {
    return (m == 0 ? lnp_rcx_message != 0 : lnp_rcx_message == m);
}

//! send a standard firmware message
/*! \return 0 on success.
 */
int send_msg(unsigned char msg)
{
  int r;
#if defined(CONF_MM)
  char* buffer_ptr = malloc(9);
#else // CONF_MM
  char* buffer_ptr = lnp_buffer;
#endif // CONF_MM
  buffer_ptr[0]=0x55;
  buffer_ptr[1]=0xff;
  buffer_ptr[2]=0x00;
  buffer_ptr[3]=0xf7;
  buffer_ptr[4]=0x08;
  buffer_ptr[5]=msg;
  buffer_ptr[6]=(unsigned char) (0xff-msg);
  buffer_ptr[7]=(unsigned char) (0xf7+msg);
  buffer_ptr[8]=(unsigned char) (0x08-msg);
  r = lnp_logical_write(buffer_ptr,9);
#if defined(CONF_MM)
  free(buffer_ptr); 
#endif // CONF_MM
  return r;
}
#endif

#endif // CONF_LNP
