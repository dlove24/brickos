/*! \file   tm.c
    \brief  Task management
    \author Markus L. Noga <markus@noga.de>
    
    Contains the multitasking switcher and scheduler as
    well as library functions relating to task management.
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
 *                  Ben Laurie <ben@algroup.co.uk>
 *                  Lou Sortman <lou (at) sunsite (dot) unc (dot) edu>
 */

#include <sys/tm.h>

#ifdef CONF_TM

#include <sys/mm.h>
#include <sys/time.h>
#include <sys/irq.h>
#include <sys/bitops.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

#ifdef CONF_VIS
# include <sys/lcd.h>
# include <conio.h>
#endif

#define fatal(a)
// #define fatal(a)  { lcd_clear(); cputs(a); lcd_refresh(); while(1); }

///////////////////////////////////////////////////////////////////////////////
//
// Variables
//
///////////////////////////////////////////////////////////////////////////////

pchain_t *priority_head;		//!< head of process priority chain

pdata_t pd_single;			//!< single process process data
pdata_t *pd_idle;			//!< idle proces
pdata_t *cpid; 			      	//!< ptr to current process data

unsigned nb_tasks;		      	//!< number of tasks

sem_t task_sem;       	      	      	//!< task data structure protection

///////////////////////////////////////////////////////////////////////////////
//
// Functions
//
///////////////////////////////////////////////////////////////////////////////

#if 0
void integrity_check(void) {
  pchain_t *prio=priority_head;
  pdata_t *pd;
  
  if(prio->prev!=NULL) {  fatal("ERR10");  }
  
  do {
    if(prio->next) {
      if(prio->next->prev!=prio) { fatal("ERR11"); }
      if(prio->next->priority>prio->priority) { fatal("ERR12"); }
    }
    pd=prio->cpid;
    do {
      if(pd==NULL) { fatal("ERR13"); }
      if(pd->priority!=prio) { fatal("ERR14"); }
      if(pd->next->prev != pd) { fatal("ERR15"); }
      if(pd->prev->next != pd) { fatal("ERR16"); }
      
      pd=pd->next;
    } while(pd!=prio->cpid);
    
    prio=prio->next;
  } while(prio);
} 
#endif 
    

//! the task switcher
/*! saves active context and passes sp to scheduler
    then restores new context from returned sp
*/
void tm_switcher(void);
__asm__("
.text
.align 1
.globl _tm_switcher
_tm_switcher:
      ; r6 saved by ROM
      ; r0 saved by system timer handler

      mov.w	r1,@-r7			; save registers
      mov.w	r2,@-r7		
      mov.w	r3,@-r7		
      mov.w	r4,@-r7		
      mov.w	r5,@-r7		

      mov.w	r7,r0			; pass sp

      jsr	_tm_scheduler		; call scheduler

_tm_switcher_return:		
      mov.w	r0,r7			; set new sp

      mov.w	@r7+,r5
      mov.w	@r7+,r4
      mov.w	@r7+,r3
      mov.w	@r7+,r2
      mov.w	@r7+,r1

      ; r0 will be restored by system timer handler
      ; r6 will be restored by ROM

      rts				; return to new task
");


//! the process scheduler
/*! \param old_sp current task's current stack pointer
    \return    new task's current stack pointer
  
    actual context switches performed by tm_switcher (assembler wrapper)
*/
size_t *tm_scheduler(size_t *old_sp) {
  pdata_t  *next; 			      	// next process to execute
  pchain_t *priority;
  wakeup_t tmp;
  
#ifdef CONF_VIS
  static unsigned char sequence=0;

#ifdef CONF_TM_DEBUG
  if(dkey & KEY_VIEW) {
    cputw( *(old_sp + SP_RETURN_OFFSET ) );
    lcd_refresh();
  }
#endif
  if(cpid!=pd_idle) {
    sequence++;
    if(sequence==10) {
      lcd_show(man_stand);
#ifndef CONF_LCD_REFRESH
      lcd_refresh();
#endif
    } else if(sequence==20) {
      lcd_show(man_run);
#ifndef CONF_LCD_REFRESH
      lcd_refresh();
#endif		  
      sequence=0;
    }
  }
#endif	// CONF_VIS
  
  if(sem_trywait(&task_sem)) {
    // process is manipulating task data structures
    // let it complete
    return old_sp;
  }
    
  priority=cpid->priority;
  switch(cpid->pstate) {
    case P_ZOMBIE:
      if(cpid->next!=cpid) {
	// remove from chain for this priority level
	//

	priority->cpid  =cpid->prev;
	cpid->next->prev=cpid->prev;
	cpid->prev->next=cpid->next;
      } else {
	// remove priority chain for this priority level
	//

	if(priority->next)
	  priority->next->prev = priority->prev;
	if(priority->prev)
	  priority->prev->next = priority->next;
	else
	  priority_head = priority->next;
	free(priority);
      }

      // We're on that stack frame being freed right now,
      // but nobody can interrupt us anyways.
      //
      free(cpid->stack_base);	        // free stack
      free(cpid);		        // free process data

      //
      // FIXME: exit code?
      //

      switch(--nb_tasks) {
	case 1:
  	  // only the idle process remains
	  if(priority_head->cpid!=pd_idle) {
	    fatal("ERR00");
	  }
	  *((pd_idle->sp_save) + SP_RETURN_OFFSET ) = (size_t) &exit;
	  pd_idle->pstate=P_SLEEPING;
	  break;
	  
	case 0:
	  // the last process has been removed
	  // -> stop switcher, go single tasking
	  
    	  systime_set_switcher(&rom_dummy_handler);
	  cpid=&pd_single;
	  
	  sem_post(&task_sem);
	  return cpid->sp_save;
      }
      break;

    case P_RUNNING:
      cpid->pstate=P_SLEEPING;
      // no break

    case P_WAITING:
      cpid->sp_save=old_sp;
  }


  // find next process willing to run
  //	
  priority=priority_head;
  next=priority->cpid->next;
  while(1) {
    if(next->pstate==P_SLEEPING)
      break;
    
    if(next->pstate==P_WAITING) {
      tmp = next->wakeup(next->wakeup_data);
      if( tmp != 0) {
	next->wakeup_data = tmp;
	break;
      }
    }
    
    if(next == priority->cpid) {
      // if we've scanned the whole chain,
      // go to next priority
      
      if(priority->next != NULL) 
	priority = priority->next;
      else {
	// FIXME: idle task has died
	//        this is a severe error.
      	fatal("ERR01");
      }
      next=priority->cpid->next;
    } else
      next=next->next;
  }
  cpid=next->priority->cpid=next;	// execute next process
  cpid->pstate=P_RUNNING;

  sem_post(&task_sem);
  return cpid->sp_save;
}

//! yield the rest of the current timeslice
/*! doesn't speed up the system clock.
*/
extern void yield(void);
__asm__("
.text
.globl _yield
.align 1
_yield:
      stc     ccr,r0h		      ; to fake an IRQ, we have to
      push    r0		      ; store the registers
      orc     #0x80,ccr               ; disable interrupts

      push    r6                      ; store r6

      mov.w   #0x04d4,r0              ; store rom return addr
      push    r0

      push    r0                      ; store r0 (destroyed by call.)

      mov.w   #_systime_tm_return,r0  ; store systime return addr
      push    r0

      jmp @_tm_switcher               ; call task switcher
");
	

//! the idle task
/*! infinite sleep instruction to conserve power.
*/
extern int tm_idle_task(int argc,char **argv) __attribute__ ((noreturn));
__asm__("
.text
.align 1
_tm_idle_task:
      sleep
      bra _tm_idle_task
");
			
//! init task management
/*! called in single tasking mode before task setup.
*/
void tm_init(void) {
   // no tasks right now.
   //
   nb_tasks=0;
   priority_head=NULL;
   sem_init(&task_sem,0,1);
   
   // the single tasking context
   //
   pd_single.pstate=P_RUNNING;
   cpid=&pd_single;

   // the idle process is an institution
   //	
   pd_idle=(pdata_t*)execi(&tm_idle_task,0,NULL,0,IDLE_STACK_SIZE);

   systime_set_timeslice(TM_DEFAULT_SLICE);
}	


//! start task management 
/*! called in single tasking mode after task setup
*/
void tm_start(void) {
  disable_irqs();			// no interruptions, please

  systime_set_switcher(&tm_switcher);
  yield();				// go!

  enable_irqs();			// restored state would 
					// disallow interrupts
}

//! execute a memory image.
/*! \param code_start start address of code to execute
    \param argc       first argument passed, normally number of strings in argv
    \param argv       second argument passed, normally pointer to argument pointers.
    \param priority   new task's priority
    \param stack_size stack size for new process
    \return -1: fail, else pid.
    
    will return to caller in any case.
*/
pid_t execi(int (*code_start)(int,char**),int argc, char **argv,
            priority_t priority,size_t stack_size) {
  pdata_t *pd;
  pchain_t *pchain, *ppchain;	// for traversing priority chain
  pchain_t *newpchain;      	// for allocating new priority chain
  size_t *sp;
  int freepchain=0;
  
  // get memory
  //
  // task & stack area belong to parent process
  // they aren't freed by mm_reaper()
  //
  if((pd=malloc(sizeof(pdata_t)))==NULL)	
    return -1;
  if((sp=malloc(stack_size))==NULL) {
    free(pd);
    return -1;
  }
  // avoid deadlock of memory and task semaphores
  // by preallocation.
  if((newpchain=malloc(sizeof(pchain_t)))==NULL) {
    free(pd);
    free(sp);
    return -1;
  }
  
  pd->pflags = 0;
  if ((size_t)code_start < (size_t)&mm_start)
    pd->pflags |= T_KERNEL;

  pd->stack_base=sp;		// these we know already.

  sp+=(stack_size>>1);		// setup initial stack

  // when main() returns a value, it passes it in r0
  // as r0 is also the register to pass single int arguments by
  // gcc convention, we can just put the address of exit on the stack.

  *(--sp)=(size_t) &exit;

  // we have to construct a stack stub so tm_switcher,
  // systime_handler and the ROM routine can fill the 
  // right values on startup.

  *(--sp)=(size_t) code_start;	// entry point    < these two are for
  *(--sp)=0;			// ccr            < rte in ROM
  *(--sp)=0;			// r6             < pop r6 in ROM
  *(--sp)=(size_t)
          &rom_ocia_return;	// ROM return     < rts in systime_handler

  *(--sp)=(size_t) argc;        // r0             < pop r0 in systime handler
  *(--sp)=(size_t)              
          &systime_tm_return;   // systime return < rts in tm_switcher

  *(--sp)=(size_t) argv;	// r1..r5	  < pop r1..r5 in tm_switcher
  *(--sp)=0;
  *(--sp)=0;
  *(--sp)=0;
  *(--sp)=0;

  pd->sp_save=sp;		// save sp for tm_switcher
  pd->pstate=P_SLEEPING;	// task is waiting for execution
  pd->parent=cpid;		// set parent

  sem_wait(&task_sem);

  ppchain=NULL;
  for(	pchain = priority_head;
	  pchain != NULL && (pchain->priority) > priority;
	  ppchain = pchain, pchain = pchain->next
	  );
  if(pchain==NULL || pchain->priority!=priority) {
    // make new chain
    //
    newpchain->priority=priority;
    newpchain->cpid=pd;

    newpchain->next=pchain;
    if(pchain)
      pchain->prev =newpchain;
    newpchain->prev=ppchain;
    if(ppchain)
      ppchain->next=newpchain;
    else
      priority_head=newpchain;

    // initial queue setup
    //
    pd->prev=pd->next=pd;
    pd->priority=newpchain;
  } else {
    // add at back of queue
    //
    pd->priority=pchain;
    pd->prev=pchain->cpid->prev;
    pd->next=pchain->cpid;
    pd->next->prev=pd->prev->next=pd;
    freepchain=1;   // free superfluous pchain.
  }
  nb_tasks++;
  
  sem_post(&task_sem);

  if(freepchain)
    free(newpchain);
  
  return (pid_t) pd;		// pid = (pid_t) &pdata_t_struct
}


//! exit task, returning code
/*! \param code The return code
 
    FIXME: for now, scrap the code.
*/
void exit(int code) {
  enable_irqs();				// just in case...
  if (!(cpid->pflags & T_KERNEL))
    mm_reaper();
  cpid->pstate=P_ZOMBIE;
  while(1)
    yield();
}


//! suspend process until wakeup function is non-null
/*! \param wakeup the wakeup function. called in task scheduler context.
    \param data argument passed to wakeup function by scheduler
    \return return value passed on from wakeup
*/
wakeup_t wait_event(wakeup_t (*wakeup)(wakeup_t),wakeup_t data) {
  cpid->wakeup     =wakeup;
  cpid->wakeup_data=data;
  cpid->pstate     =P_WAITING;

  yield();

  return cpid->wakeup_data;
}


//! wakeup function for sleep
/*! \param data time to wakeup encoded as a wakeup_t
*/
static wakeup_t tm_sleep_wakeup(wakeup_t data) {
	return ((time_t)data)<=sys_time;
}

//! delay execution allowing other tasks to run.
/*! \param msec sleep duration in milliseconds
   \return number of milliseconds left if interrupted, else 0.
   \bug interruptions not implemented.
 */
unsigned int msleep(unsigned int msec)
{
#if defined(CONF_TIME) && defined(CONF_TM)
  (void) wait_event(&tm_sleep_wakeup, sys_time + MSECS_TO_TICKS(msec));
#else
  delay(msec);
#endif
  return 0;
}

//! delay execution allowing other tasks to run.
/*! \param sec sleep duration in seconds
   \return number of seconds left if interrupted, else 0.
   \bug interruptions not implemented.
 */
unsigned int sleep(unsigned int sec)
{
  msleep(1000*sec);
  return 0;
}


//! kill a process
/*! \param pid must be valid process ID, or undefined behaviour will result!
*/
void kill(pid_t pid) {
  pdata_t *pd=(pdata_t*) pid;
  if(pd==cpid)
    exit(-1);
  else {
    // when the task is switched to the next time,
    // make it exit immediatlely.
    
    sem_wait(&task_sem);
    *( (pd->sp_save) + SP_RETURN_OFFSET )=(size_t) &exit;
    pd->pstate=P_SLEEPING;		// in case it's waiting.
    sem_post(&task_sem);
  }
}

void killall(priority_t prio) {
  pchain_t *pchain;
  pdata_t  *pd;

  sem_wait(&task_sem);

  // find first chain with smaller or equal priority.
  //
  pchain=priority_head;
  while(pchain!=NULL && prio<pchain->priority)
    pchain=pchain->next;

  while(pchain!=NULL) {
    pd=pchain->cpid;
    do {
      if(pd!=cpid && pd!=pd_idle) {
	// kill it
	//
	*( (pd->sp_save) + SP_RETURN_OFFSET )=(size_t) &exit;
        pd->pstate=P_SLEEPING;		// in case it's waiting.
      }
      pd=pd->next;
    } while(pd!=pchain->cpid);
    pchain=pchain->next;
  }    

  sem_post(&task_sem);
}

#endif // CONF_TM

