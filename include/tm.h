/*! \file include/tm.h
    \brief  Interface: task management
    \author Markus L. Noga <markus@noga.de>

    Defines types and flags used in task management.
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
 *                  Lou Sortman <lou (at) sunsite (dot) unc (dot) edu>
 */

#ifndef __tm_h__
#define __tm_h__

#ifdef  __cplusplus
extern "C" {
#endif // __cplusplus

#include <config.h>
#include <mem.h>

///////////////////////////////////////////////////////////////////////
//
// Definitions
//
///////////////////////////////////////////////////////////////////////

typedef volatile unsigned char tstate_t;                 //!< task state type
typedef volatile unsigned char tflags_t;                 //!< task flags type

typedef unsigned char priority_t;               //!< task priority type

//! The lowest possible task priority
#define PRIO_LOWEST        1
//! The priority of most tasks
#define PRIO_NORMAL       10
//! The highest possible task priority
#define PRIO_HIGHEST      20

typedef unsigned long wakeup_t;                 //!< wakeup data area type

//
//! task states
/*! \todo the following comments on the defined may cause problems in
 * when used in macros/expressions, etc.  FixEm?
*/

#define T_DEAD      0                           //!< dead and gone, stack freed
#define T_ZOMBIE    1                           //!< terminated, cleanup pending
#define T_WAITING   2                           //!< waiting for an event
#define T_SLEEPING  3                           //!< sleeping. wants to run.
#define T_RUNNING   4                           //!< running

//
//! task flags
//

#define T_KERNEL  	(1 << 0)                    //!< kernel task
#define T_USER    	(1 << 1)                    //!< user task
#define T_IDLE    	(1 << 2)                    //!< idle task
#define T_SHUTDOWN	(1 << 7)                    //!< shutdown requested


#define DEFAULT_STACK_SIZE  512                 //!< that's enough.

#ifndef DOXYGEN_SHOULD_SKIP_INTERNALS
/**
 * priority chain data structure
 * @internal
 */
struct _pchain_t {
  priority_t priority;                          //!< numeric priority level

  struct _pchain_t *next;                       //!< lower priority chain
  struct _pchain_t *prev;                       //!< higher priority chain

  struct _tdata_t *ctid;                        //!< current task in chain
};

/** priority chain data type
 *  a shorthand
 */
typedef struct _pchain_t pchain_t;

/** task data structure
 * @internal
 */
struct _tdata_t {
  size_t *sp_save;                              //!< saved stack pointer

  tstate_t tstate;                              //!< task state
  tflags_t tflags;                              //!< task flags
  pchain_t *priority;                           //!< priority chain

  struct _tdata_t *next;                        //!< next task in queue
  struct _tdata_t *prev;                        //!< previous task in queue
  struct _tdata_t *parent;                      //!< parent task

  size_t *stack_base;                           //!< lower stack boundary

  wakeup_t(*wakeup) (wakeup_t);                 //!< event wakeup function
  wakeup_t wakeup_data;                         //!< user data for wakeup fn
};

//! task data type
/*! a shorthand
 */
typedef struct _tdata_t tdata_t;

#endif // DOXYGEN_SHOULD_SKIP_INTERNALS 

#if defined(CONF_TM)
//! test to see if task has been asked to shutdown
/*! Check task shutdown flag.  If set, the task should shutdown
 *  as soon as possible.  If clear, continue running.
 */
#define shutdown_requested() ((ctid->tflags & T_SHUTDOWN) != 0)
extern tdata_t *ctid;
#else // CONF_TM
#define shutdown_requested() (0)
#endif // CONF_TM

//! task id type
/*! In effect, the kernel simply typecasts *tdata_t to tid_t.
 */
typedef size_t tid_t;

#ifdef  __cplusplus
}
#endif // __cplusplus

#endif
