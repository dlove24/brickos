/*! \file   include/lnp/sys/lnp-logical.h
    \brief  Internal LNP Interface: link networking protocol logical layer
    \author Markus L. Noga <markus@noga.de>
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

#ifndef __sys_lnp_logical_h__
#define __sys_lnp_logical_h__

#ifdef  __cplusplus
extern "C" {
#endif

#include "../lnp-logical.h"

#ifdef CONF_LNP

#ifndef CONF_HOST
#include <time.h>
#else
#define MSECS_TO_TICKS(a)   (a)
#endif

///////////////////////////////////////////////////////////////////////
//
// Definitions
//
///////////////////////////////////////////////////////////////////////

#ifdef  CONF_LNP_FAST
#define LNP_LOGICAL_BAUD_RATE   B4800             //!< baud rate
#define LNP_LOGICAL_PARITY  SMR_P_NONE        //!< parity
#define LNP_BYTE_TIME           MSECS_TO_TICKS(3) //!< time to transmit a byte
#else
#define LNP_LOGICAL_BAUD_RATE   B2400             //!< baud rate
#define LNP_LOGICAL_PARITY  SMR_P_ODD         //!< parity
#define LNP_BYTE_TIME           MSECS_TO_TICKS(5) //!< time to transmit a byte
#endif

#define LNP_BYTE_TIMEOUT        (3*LNP_BYTE_TIME/2) //!< timeout waiting for a byte
#define LNP_BYTE_SAFE       (4*LNP_BYTE_TIME) //!< delay before transmitting a byte

#define LNP_WAIT_TXOK       (2*LNP_BYTE_TIMEOUT) //!< delay after good transmit
#define LNP_WAIT_COLL           (4*LNP_BYTE_TIMEOUT) //!< delay after collision


#define TX_COLL   (-1)    //!< not transmitting, last xmit was collision
#define TX_IDLE   ( 0)    //!< not transmitting, last xmit OK
#define TX_ACTIVE ( 1)    //!< currently transmitting


///////////////////////////////////////////////////////////////////////
//
// Variables
//
///////////////////////////////////////////////////////////////////////

extern volatile signed char tx_state;   //!< transmit status


///////////////////////////////////////////////////////////////////////
//
// Functions
//
///////////////////////////////////////////////////////////////////////

//! Initialize the logical layer (IR port)
//
extern void lnp_logical_init(void);


//! Shutdown the logical layer (IR port)
//
extern void lnp_logical_shutdown(void);

//! Callback: end of transmission
//
void txend_handler(void)
#ifdef RCX_COMPILER
 __attribute__ ((rcx_interrupt))
#endif
;

#endif  // CONF_LNP

#ifdef  __cplusplus
}
#endif

#endif  // __sys_lnp_logical_h__

