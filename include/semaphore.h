/*! \file include/semaphore.h
    \brief  Interface: POSIX 1003.1b semaphores for task synchronization
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
 *                  Henner Zeller <H.Zeller@acm.org> (sem_timedwait())
 */

#ifndef __semaphore_h__
#define __semaphore_h__

#ifdef  __cplusplus
extern "C" {
#endif

#include <config.h>
#include <time.h> /* time_t */
#include <atomic.h>

#ifdef CONF_SEMAPHORES

///////////////////////////////////////////////////////////////////////
//
// Definitions
//
///////////////////////////////////////////////////////////////////////

typedef atomic_t sem_t;                         //!< the semaphore data-type

#define EAGAIN  0xffff                          //!< the error code

///////////////////////////////////////////////////////////////////////
//
// Functions
//
///////////////////////////////////////////////////////////////////////

//! Initialize a semaphore
/*! sem_init() initializes the semaphore object  pointed  to  by
 *  {sem} by setting its internal count to {value}
 *  \param sem a pointer to the semaphore to be initialized
 *  \param value the initial value for count
 *  \param pshared (this argument is ignored)
 *  \return (always 0)
*/
extern inline int sem_init(sem_t * sem, int pshared, unsigned int value)
{
  *sem = (sem_t) value;
  return 0;
}

//! Wait for semaphore (blocking)
/*!  sem_wait()  suspends  the calling task until the semaphore
 *   pointed to by {sem} has non-zero count. It  then  atomically
 *   decreases the semaphore count.
 *  \param sem a pointer to the semaphore on which to wait
 *  \return 0 if wait returned immediately, EAGAIN if task did have to 
 *  wait for the semaphore.
*/
extern int sem_wait(sem_t * sem);

/**
 * Wait for semaphore (blocking with timeout).
 *
 * sem_timedwait() suspends the calleing task until either the semaphore
 * pointed to by {sem} has non-zero count or the given absolute timeout
 * passed. Note the timeout is an ABSOLUTE time not relative (yes the standard
 * is that stupid); so if you want a relative waiting time, call the
 * function with <code>get_system_up_time() + relativeTime</code>.
 *
 * If the semaphore reached a non-zero count its value is then atomically
 * decreased.
 *
 * @param sem a pointer to the semaphore on which to wait
 * @param abs_timeout the absolute timeout of this operation. If the
 *        semaphore cannot be locked up to this time, this function returns.
 * @return 0 if the semaphore could successfully be locked; -1 if the
 *         timeout has been reached.
 * @note in IEEE 1003.1, the timeout is passed as a struct timeval not
 *       a time_t.
 */
extern int sem_timedwait(sem_t * sem,
			 const time_t abs_timeout);

//! Try a wait for semaphore (non-blocking)
/*! sem_trywait() is a non-blocking variant of sem_wait().  If the
 *  semaphore pointed to by {sem} has non-zero count, the  count
 *  is   atomically decreased  and  sem_trywait  immediately
 *  returns 0.  If the semaphore count  is  zero,  sem_trywait
 *  immediately returns with error EAGAIN.
 *  \param sem a pointer to the semaphore on which to attempt a wait
 *  \return 0 if decremented the semaphore or EAGAIN if can't
 *
 *  NOTE: this is IRQ handler safe.
*/
extern int sem_trywait(sem_t * sem);

//! Post a semaphore
/*! sem_post()  atomically  increases the count of the semaphore
 *  pointed to by {sem}.  This function  never  blocks  and  can
 *  safely be used in asynchronous signal handlers.
 *  \param sem a pointer to the semaphore to be signaled
 *  \return (always returns 0)
*/
extern inline int sem_post(sem_t * sem) 
{ 
	atomic_inc(sem);
	return 0;
}

//
//! Get the semaphore value
//
extern inline int sem_getvalue(sem_t * sem, int *sval)
{
  *sval = *sem;
  return 0;
}

//! We're done with the semaphore, destroy it 
/*! sem_destroy()  destroys  a  semaphore  object,  freeing  the
 *  resources  it  might hold. 
 *  \param sem a pointer to the semaphore to be destroyed
 *  \return (always returns 0)
 *
 *  NOTE: No tasks should be waiting on
 *  the semaphore at the time sem_destroy is  called.
*/
extern inline int sem_destroy(sem_t * sem)
{
  return 0;
}

#endif // CONF_SEMAPHORES

#ifdef  __cplusplus
}
#endif

#endif // __semaphore_h__
