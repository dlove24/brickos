/*! \file   include/sys/tm.h
    \brief  Internal Interface: task management
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

///////////////////////////////////////////////////////////////////////
//
// Variables
//
///////////////////////////////////////////////////////////////////////

extern tdata_t td_single;		//!< single process process data

extern tdata_t *ctid;			//!< ptr to current process data

extern volatile unsigned int nb_tasks;		//!< number of tasks

  // nb_system_tasks is maintained  in execi and kill/killall
  // T_KERNEL tasks are counted as they are started
extern volatile unsigned int nb_system_tasks;

  // tm_timeslice is from kernel/systime.c
extern volatile unsigned char tm_timeslice;	//!< task time slice


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

