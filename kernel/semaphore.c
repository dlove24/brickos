/*! \file   semaphore.c
    \brief  Implementation: POSIX 1003.1b semaphores for process synchronization.
    \author Markus L. Noga <markus@noga.de>
*/
    
/*
 *  The contents of this file are subject to the Mozilla Public License
 *  Version 1.0 (the "License"); you may not use this file except in
 *  compliance with the License. You may obtain a copy of the License at
 *  http://www.mozilla.org/MPL/
 *
 *  Software distributed under the License is distributed on an "AS IS"
 *  basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 *  License for the specific language governing rights and limitations
 *  under the License.
 *
 *  The Original Code is legOS code, released October 17, 1999.
 *
 *  The Initial Developer of the Original Code is Markus L. Noga.
 *  Portions created by Markus L. Noga are Copyright (C) 1999
 *  Markus L. Noga. All Rights Reserved.
 *
 *  Contributor(s): Markus L. Noga <markus@noga.de>
 */

#include <semaphore.h>

#ifdef CONF_SEMAPHORES

#include <unistd.h>

///////////////////////////////////////////////////////////////////////////////
//
// Functions
//
///////////////////////////////////////////////////////////////////////////////

//! the semaphore event wakeup function for wait_event().
/*! \param data pointer to the semaphore passed as a wakeup_t
*/
wakeup_t sem_event_wait(wakeup_t data) {
	sem_t *sem=(sem_t*) ((unsigned)data);
	
	// we're called by the scheduler, therefore in an IRQ handler,
	// so no worrying about IRQs.
	//
	if(*sem) {
		(*sem)--;
		return 1;			// sem!=0 -> wakeup
	}
	return 0;
}

//! wait on a semaphore
/*! \param sem a valid semaphore

    suspends the calling thread until the semaphore
    has non-zero count. It  then  atomically
    decreases the semaphore count.
    
    implemented with wait_event().                 
*/
int sem_wait(sem_t * sem) {
	// check if semaphore is available, if not, go to sleep
	
	if(sem_trywait(sem))
		if (wait_event(sem_event_wait,(unsigned long) ((unsigned)sem)) == 0)
			return -1;
	
	return 0;
}

typedef struct {
	sem_t  *sem;
	time_t abs_timeout;
} timeout_sem_data_t;

static wakeup_t sem_event_timeout_wait(wakeup_t data) {
	timeout_sem_data_t *tsem = (timeout_sem_data_t*) ((unsigned)data);
	
	// we're called by the scheduler, therefore in an IRQ handler,
	// so no worrying about IRQs.
	//
	if (*tsem->sem) {
		(*tsem->sem)--;
		return 1;			// sem!=0 -> wakeup
	}
	
	if (tsem->abs_timeout <= get_system_up_time()) {
		return 2;                       // timeout reached -> wakeup
	}

	return 0;
}

int sem_timedwait(sem_t *sem,
		  const time_t abs_timeout) {
	timeout_sem_data_t data;
	data.sem = sem;
	data.abs_timeout = abs_timeout;
	
	if (sem_trywait(sem)) {
		if (wait_event(sem_event_timeout_wait, 
			       (wakeup_t) ((unsigned) &data)) != 1) {
			return -1; // timeout reached.
		}
	}
	return 0;
}

//! non-blocking check on a semaphore
/*! \param sem a valid semaphore

    a non-blocking variant of sem_wait.  If the
    semaphore has non-zero count, the  count
    is   atomically   decreased  and  sem_trywait  immediately
    returns 0.  If the semaphore count  is  zero,  sem_trywait
    immediately returns with error EAGAIN.
   
    this is IRQ handler safe.
*/
int sem_trywait(sem_t * sem);
#ifndef DOXYGEN_SHOULD_SKIP_THIS
__asm__("\n\
.text\n\
.align 1\n\
.globl _sem_trywait\n\
       _sem_trywait:\n\
	stc ccr,r1h				; save flags	\n\
	orc #0x80,ccr				; block all but NMI\n\
	mov.b @r0,r1l\n\
	beq sem_fail				; !=0 -> decrease, return 0\n\
	dec r1l\n\
	mov.b r1l,@r0\n\
	sub.w r0,r0                             ; return 0\n\
	bra sem_ok\n\
\n\
 sem_fail:\n\
        mov #0xffff,r0	    		      	; else return 0xffff\n\
\n\
 sem_ok:\n\
        ldc r1h,ccr				; restore flags\n\
	rts\n\
	");
#endif // DOXYGEN_SHOULD_SKIP_THIS
	
#endif // CONF_SEMAPHORES
