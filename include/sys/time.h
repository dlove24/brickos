/*! \file   include/sys/time.h
    \brief  Internal Interface: system time functions
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

#ifndef __sys_time_h__
#define __sys_time_h__

#ifdef  __cplusplus
extern "C" {
#endif

#include "../time.h"

#ifdef CONF_TIME

///////////////////////////////////////////////////////////////////////
//
// Definitions
//
///////////////////////////////////////////////////////////////////////

#ifdef CONF_TM
#define TM_DEFAULT_SLICE 20	//!< default multitasking timeslice
#endif


///////////////////////////////////////////////////////////////////////
//
// Global variables
//
///////////////////////////////////////////////////////////////////////

#ifdef CONF_TM
//! return address for the task switcher
//
extern void* systime_tm_return;
#endif


///////////////////////////////////////////////////////////////////////
//
// Functions
//
///////////////////////////////////////////////////////////////////////

//! initialize system timer
/*! task switcher initialized to empty handler,
    motors turned off
*/
void systime_init(void);

//! shutdown system timer
//
void systime_shutdown(void);

#ifdef CONF_TM
//! set task switcher vector
//
void systime_set_switcher(void* switcher);

//! set multitasking timeslice in ms
//
void systime_set_timeslice(unsigned char slice);
#endif	// CONF_TM

time_t get_system_up_time(void);
#endif  // CONF_TIME

#ifdef  __cplusplus
}
#endif

#endif	// __sys_time_h__

