/*! \file   include/unistd.h
    \brief  reduced UNIX standard library
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

#ifndef __unistd_h__
#define __unistd_h__

#ifdef  __cplusplus
extern "C" {
#endif

#include <tm.h>
#include <time.h>

///////////////////////////////////////////////////////////////////////
//
// Functions
//
///////////////////////////////////////////////////////////////////////

//
// execute an image
// args: start address of code to execute
//       stack size for new process.
// retval: -1: fail, else pid
//
extern pid_t execi(int (*code_start) (int, char **), int argc, char **argv,
		   priority_t priority, size_t stack_size);

//
// exit task, returning code.
//
extern void exit(int code) __attribute__((noreturn));

//
// yield the rest of the current timeslice
//
extern void yield(void);

//
// suspend process until wakeup function is non-null
// wakeup function is called in task scheduler context
// retval: wakeup() return value
//
#ifdef CONF_TM
extern wakeup_t wait_event(wakeup_t(*wakeup) (wakeup_t), wakeup_t data);

//! delay execution allowing other tasks to run.
/*! \param sec sleep duration in seconds
   \return number of seconds left if interrupted, else 0.
   \bug interruptions not implemented.
 */
extern unsigned int sleep(unsigned int sec);

//! delay execution allowing other tasks to run.
/*! \param msec sleep duration in milliseconds
   \return number of milliseconds left if interrupted, else 0.
   \bug interruptions not implemented.
 */
extern unsigned int msleep(unsigned int msec);
#else
extern inline wakeup_t wait_event(wakeup_t(*wakeup) (wakeup_t), wakeup_t data)
{
  wakeup_t res;

  while (!(res = wakeup(data)))
	/* wait */;
  return res;
}

// Replacement for sleep/msleep if no TM
#define	sleep(s)	delay(1000*(s))
#define msleep(s)	delay(s)
#endif

//! kill a process
// FIXME: this belongs in a different header
//
extern void kill(pid_t pid);

//! kill all processes with priority lower or equal than p, excluding self.
extern void killall(priority_t p);

#ifdef  __cplusplus
}
#endif

#endif // __unistd_h__
