/*! \file   lnp-logical.c
    \brief  Implementation: link networking protocol logical layer
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
 *                  Chris Dearman <chris@algor.co.uk>
 *                  Martin Cornelius <Martin.Cornelius@t-online.de>
 */

#include <sys/lnp-logical.h>

#ifdef CONF_LNP

#include <sys/lnp.h>

#include <sys/h8.h>
#include <sys/irq.h>

#ifdef CONF_AUTOSHUTOFF
#include <sys/timeout.h>
#endif

#include <time.h>
#include <mem.h>
#include <semaphore.h>
#include <unistd.h>

#include <rom/registers.h>

///////////////////////////////////////////////////////////////////////////////
//
// Variables
//
///////////////////////////////////////////////////////////////////////////////

static const unsigned char *tx_ptr; //!< ptr to next byte to transmit
static const unsigned char *tx_verify;  //!< ptr to next byte to verify
static const unsigned char *tx_end; //!< ptr to byte after last

volatile signed char tx_state;    //!< flag: transmission state

//! when to allow next transmission
/*! transmit OK -> wait some time to grant others bus access
    collision   -> wait some more, plus "random" amount.
    receive     -> reset timeout to default value.
*/

static time_t allow_tx;                 //!< time to allow new transmission

#ifdef CONF_TM
static sem_t tx_sem;                //!< transmitter access semaphore
#endif

///////////////////////////////////////////////////////////////////////////////
//
// Functions
//
///////////////////////////////////////////////////////////////////////////////

#ifdef CONF_RCX_COMPILER
static void rx_handler(void) __attribute__ ((rcx_interrupt));
static void rxerror_handler(void) __attribute__ ((rcx_interrupt));
static void tx_handler(void) __attribute__ ((rcx_interrupt));
       void txend_handler(void) __attribute__ ((rcx_interrupt));
#else
void rx_handler(void);
void rxerror_handler(void);
void tx_handler(void);
void txend_handler(void);
#endif


//! enable IR carrier frequency.
extern inline void carrier_init(void) {
  T1_CR  =0x9;
  T1_CSR =0x13;
  T1_CORA=0x1a;
}

//! disable IR carrier frequency.
extern inline void carrier_shutdown(void) {
  T1_CR  =0;
  T1_CSR =0;
}

//! the byte received interrupt handler
//
#ifdef CONF_RCX_COMPILER
static void rx_handler(void) {
#else
HANDLER_WRAPPER("rx_handler","rx_core");
void rx_core(void) {
#endif
  time_t new_tx;
  lnp_timeout_reset();
  if(tx_state<TX_ACTIVE) {
    // foreign bytes
    //
    new_tx = get_system_up_time()+LNP_BYTE_SAFE;
    if (new_tx > allow_tx) allow_tx = new_tx;
    lnp_integrity_byte(S_RDR);
  } else {
    // echos of own bytes -> collision detection
    //
    if(S_RDR!=*tx_verify) {
      txend_handler();
      tx_state=TX_COLL;
    } else if( tx_end <= ++tx_verify ) {
      // let transmission end handler handle things
      //
      tx_state=TX_IDLE;
    }
  }

  // suppress volatile modifier to generate bit instruction.
  //
  *((char*) &S_SR) &=~SSR_RECV_FULL;
}

//! the receive error interrupt handler
//
#ifdef CONF_RCX_COMPILER
static void rxerror_handler(void) {
#else
HANDLER_WRAPPER("rxerror_handler","rxerror_core");
void rxerror_core(void) {
#endif
  time_t new_tx;
  if(tx_state<TX_ACTIVE) {
    lnp_integrity_reset();
    new_tx = get_system_up_time()+LNP_BYTE_SAFE;
    if (new_tx > allow_tx) allow_tx = new_tx;
  } else {
    txend_handler();
    tx_state=TX_COLL;
  }

  S_SR&=~SSR_ERRORS;
}

//! the end-of-transmission interrupt handler
//
#ifdef CONF_RCX_COMPILER
void txend_handler(void) {
#else
HANDLER_WRAPPER("txend_handler","txend_core");
void txend_core(void) {
#endif
  // shutdown transmit and irqs, clear status flags
  //
  S_CR&=~(SCR_TX_IRQ | SCR_TRANSMIT | SCR_TE_IRQ);
  S_SR&=~(SSR_TRANS_EMPTY | SSR_TRANS_END);
}

//! the transmit byte interrupt handler
/*! write next byte if there's one left, otherwise unhook irq.
*/
#ifdef CONF_RCX_COMPILER
static void tx_handler(void) {
#else
HANDLER_WRAPPER("tx_handler","tx_core");
void tx_core(void) {
#endif
  if(tx_ptr<tx_end) {
    // transmit next byte
    //
    S_TDR = *(tx_ptr++);
    *((char*) &S_SR)&=~SSR_TRANS_EMPTY;
  } else {
    // disable transmission interrupt
    //
    S_CR&=~SCR_TX_IRQ;
  }
}

//! shutdown IR port
void lnp_logical_shutdown(void) {
  S_CR =0;        // everything off
  carrier_shutdown();
  lnp_logical_range(0);

  tx_state=TX_IDLE;
  allow_tx=0;

#ifdef CONF_TM
  sem_destroy(&tx_sem);
#endif
}

//! initialize link networking protocol logical layer (IR port).
/*! initially set to low range.
*/
void lnp_logical_init(void) {
  // safe initial state.
  //
  lnp_logical_shutdown();

#ifdef CONF_TM
  sem_init(&tx_sem,0,1);
#endif

  // serial setup according to CONF_LNP_FAST / lnp_logical.h
  // was 8N1 fixed, now parity is also set.
  //
  S_MR =LNP_LOGICAL_PARITY;
  S_BRR=LNP_LOGICAL_BAUD_RATE;
  S_SR =0;

  // carrier setup
  //
  rom_port4_ddr |= 1;     // port 4 bit 0 output
  PORT4_DDR = rom_port4_ddr;
  carrier_init();
  rom_port5_ddr = 4;      // init p5ddr, for now
  PORT5_DDR = rom_port5_ddr;

  // IRQ handler setup
  //
  eri_vector=&rxerror_handler;
  rxi_vector=&rx_handler;
  txi_vector=&tx_handler;
  tei_vector=&txend_handler;

  // enable receiver and IRQs
  //
  S_CR=SCR_RECEIVE | SCR_RX_IRQ;
}


static wakeup_t write_allow(wakeup_t data) {
  return get_system_up_time() >= *((volatile time_t*)&allow_tx);
}

static wakeup_t write_complete(wakeup_t data) {
  return *((volatile signed char*)&tx_state)<TX_ACTIVE;
}

//! write to IR port, blocking.
/*! \param buf data to transmit
    \param len number of bytes to transmit
    \return 0 on success, else collision
*/
int lnp_logical_write(const void* buf,size_t len) {
  unsigned char tmp;

#ifdef CONF_TM
  if (sem_wait(&tx_sem) == -1)
  	return tx_state;
#endif

#ifdef CONF_AUTOSHUTOFF
  shutoff_restart();
#endif

	if (wait_event(write_allow,0) != 0)
	{
	  lnp_timeout_reset();

  	tx_verify=tx_ptr=buf;                 // what to transmit
	  tx_end=buf+len;

	  tx_state=TX_ACTIVE;
	  S_SR&=~(SSR_TRANS_EMPTY | SSR_TRANS_END); // clear flags
	  S_CR|=SCR_TRANSMIT | SCR_TX_IRQ | SCR_TE_IRQ; // enable transmit & irqs

	  wait_event(write_complete,0);

	  // determine delay before next transmission
	  //
	  if(tx_state==TX_IDLE)
	    tmp=LNP_WAIT_TXOK;
	  else
	    tmp=LNP_WAIT_COLL + ( ((unsigned char) 0x0f) &
	        ( ((unsigned char) len)+
	          ((unsigned char*)buf)[len-1]+
	          ((unsigned char) get_system_up_time())    ) );
	  allow_tx=get_system_up_time()+tmp;
	}

#ifdef CONF_TM
  sem_post(&tx_sem);
#endif

  return tx_state;
}

#endif  // CONF_LNP
