/*! \file   include/sys/tm.h
    \brief  task management interna
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

#ifndef __sys_tm_h__
#define __sys_tm_h__

#ifdef  __cplusplus
extern "C" {
#endif

#include <config.h>

#ifdef CONF_TM

#include "../tm.h"

///////////////////////////////////////////////////////////////////////
//
// Definitions
//
///////////////////////////////////////////////////////////////////////


#define SP_RETURN_OFFSET	10	//!< return address offset on stack in words.

#define IDLE_STACK_SIZE		128	//!< should suffice for IRQ service

#ifdef CONF_PROGRAM

#ifdef CONF_LR_HANDLER
#define NUM_SYSTEM_TASKS	4	//! idle, packet_consumer, key_handler, lr_handler
#else
#define NUM_SYSTEM_TASKS	3	//! idle, packet_consumer, key_handler
#endif

#else // CONF_PROGRAM

#define NUM_SYSTEM_TASKS	1	//! idle

#endif

///////////////////////////////////////////////////////////////////////
//
// Variables
//
///////////////////////////////////////////////////////////////////////

extern pdata_t pd_single;		//!< single process process data

extern pdata_t *cpid;			//!< ptr to current process data

extern unsigned nb_tasks;		//!< number of tasks


///////////////////////////////////////////////////////////////////////
//
// Functions
//
///////////////////////////////////////////////////////////////////////

//! init task management
/* called in single tasking mode before task setup.
*/
extern void tm_init(void);


//! start task management
/*! called in single tasking mode after task setup
*/
extern void tm_start(void);


//! the task switcher IRQ handler
/*! located in the assembler process module
*/
extern void tm_switcher(void);


//! the process scheduler
/*! \param  old_sp current task's current stack pointer
    \return new task's current stack pointer

    actual context switches performed by tm_switcher (assembler wrapper)
*/
extern size_t *tm_scheduler(size_t *old_sp);


//! the idle task
/*! infinite sleep instruction to conserve power
*/
extern int tm_idle_task(int,char**);

#endif	// CONF_TM

#ifdef  __cplusplus
}
#endif

#endif	// __sys_tm_h__

