/*! \file   include/time.h
    \brief  Time-related types
    \author Markus L. Noga <markus@noga.de>

    \bug    time_t is a 32 bit value which will overflow after
    49.7 days of continuous operation.
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

#ifndef __time_h__
#define __time_h__

#ifdef  __cplusplus
extern "C" {
#endif

#include <config.h>

#ifdef CONF_TIME

///////////////////////////////////////////////////////////////////////
//
// Definitions
//
///////////////////////////////////////////////////////////////////////

//! time type
/*! \bug This is a 32 bit value which will overflow after
    49.7 days of continuous operation.
 */
typedef unsigned long time_t;

//
// time & related stuff
//

#define TICK_IN_MS	1	// timer tick in ms
#define TICKS_PER_SEC	1000	// how many per second?

#define SECS_TO_TICKS(a)  ((a)*TICKS_PER_SEC)	// conversion functions
#define MSECS_TO_TICKS(a) ((a)/TICK_IN_MS)

///////////////////////////////////////////////////////////////////////
//
// Variables
//
///////////////////////////////////////////////////////////////////////

extern volatile time_t sys_time;	//!< current system time

#endif // CONF_TIME

#ifdef  __cplusplus
}
#endif

#endif
