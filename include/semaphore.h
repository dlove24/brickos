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
 */

#ifndef __semaphore_h__
#define __semaphore_h__

#ifdef  __cplusplus
extern "C" {
#endif

#include <config.h>

#ifdef CONF_SEMAPHORES

///////////////////////////////////////////////////////////////////////
//
// Definitions
//
///////////////////////////////////////////////////////////////////////

typedef unsigned char sem_t;                    //!< the semaphore type

#define EAGAIN  0xffff                          //!< an error code

///////////////////////////////////////////////////////////////////////
//
// Functions
//
///////////////////////////////////////////////////////////////////////

//
//  sem_init initializes the semaphore object  pointed  to  by
//  sem.   The count associated with the semaphore is set ini-
//  tially to value.
//
//  The pshared argument is ignored.
//
extern inline int sem_init(sem_t * sem, int pshared, unsigned int value)
{
  *sem = (sem_t) value;
  return 0;
}

//
//  sem_wait  suspends  the calling task until the semaphore
//  pointed to by sem has non-zero count. It  then  atomically
//  decreases the semaphore count.
//
extern int sem_wait(sem_t * sem);

//
//  sem_trywait is a non-blocking variant of sem_wait.  If the
//  semaphore pointed to by sem has non-zero count, the  count
//  is   atomically decreased  and  sem_trywait  immediately
//  returns 0.  If the semaphore count  is  zero,  sem_trywait
//  immediately returns with error EAGAIN.
//
//  this is IRQ handler safe.
//
extern int sem_trywait(sem_t * sem);

//
//  sem_post  atomically  increases the count of the semaphore
//  pointed to by sem.  This function  never  blocks  and  can
//  safely be used in asynchronous signal handlers.
//
extern int sem_post(sem_t * sem);

//
// get the semaphore value
//
extern inline int sem_getvalue(sem_t * sem, int *sval)
{
  *sval = *sem;
  return 0;
}

//
//  sem_destroy  destroys  a  semaphore  object,  freeing  the
//  resources  it  might hold. No tasks should be waiting on
//  the semaphore at the time sem_destroy is  called.
//
extern inline int sem_destroy(sem_t * sem)
{
  return 0;
}

#endif // CONF_SEMAPHORES

#ifdef  __cplusplus
}
#endif

#endif // __semaphore_h__
