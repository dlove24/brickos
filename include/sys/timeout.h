/*! \file   include/sys/timeout.h
    \brief  Internal Interface: Powerdown Timer Routines
    \author Rossz Vamos-Wentworth <rossw@jps.net>
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
 *		    Rossz Vámos-Wentworth <rossw@jps.net>
 */

#ifndef __sys_timeout_h__
#define __sys_timeout_h__

#ifdef  __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////////////////////////
//
// Definitions
//
///////////////////////////////////////////////////////////////////////
#define DEFAULT_SHUTOFF_TIME (15*60)

///////////////////////////////////////////////////////////////////////
//
// Variables
//
///////////////////////////////////////////////////////////////////////

extern volatile unsigned int auto_shutoff_counter;
extern unsigned int auto_shutoff_period;
extern unsigned int auto_shutoff_secs;
extern volatile unsigned int auto_shutoff_elapsed;
extern volatile unsigned int idle_powerdown;

///////////////////////////////////////////////////////////////////////
//
// Functions
//
///////////////////////////////////////////////////////////////////////

extern void shutoff_init(void);
extern void shutoff_restart(void);

extern void autoshutoff_check(void)
#ifdef CONF_RCX_COMPILER
__attribute__ ((rcx_interrupt))
#endif
;

#ifdef  __cplusplus
}
#endif

#endif
