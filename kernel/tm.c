/*! \file tm.c
    \brief  Implementation: Task management
    \author Markus L. Noga <markus@noga.de>
    
    The implementation of the multitasking switcher and scheduler as
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

#include <sys/critsec.h>
#include <sys/mm.h>
#include <sys/time.h>
#include <sys/irq.h>
#include <sys/bitops.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef CONF_VIS
# include <sys/lcd.h>
# include <conio.h>
# include <sys/battery.h>
#endif

#define fatal(a)
// #define fatal(a)  { lcd_clear(); cputs(a); lcd_refresh(); while(1); }

///////////////////////////////////////////////////////////////////////////////
//
// Variables
//
///////////////////////////////////////////////////////////////////////////////

pchain_t *priority_head;                        //!< head of task priority chain

tdata_t td_single;                              //!< single task data
tdata_t *ctid;                                  //!< ptr to current task data

volatile unsigned int nb_tasks;                 //!< number of tasks
volatile unsigned int nb_system_tasks;          //!< number of system (kernel) tasks

///////////////////////////////////////////////////////////////////////////////
//
// Functions
//
///////////////////////////////////////////////////////////////////////////////

#if 0
void integrity_check(void) {
  pchain_t *prio=priority_head;
  tdata_t *td;
  
  if(prio->prev!=NULL) {  fatal("ERR10");  }
  
  do {
    if(prio->next) {
      if(prio->next->prev!=prio) { fatal("ERR11"); }
      if(prio->next->priority>prio->priority) { fatal("ERR12"); }
    }
    td=prio->ctid;
    do {
      if(td==NULL) { fatal("ERR13"); }
      if(td->priority!=prio) { fatal("ERR14"); }
      if(td->next->prev != td) { fatal("ERR15"); }
      if(td->prev->next != td) { fatal("ERR16"); }
      
      td=td->next;
    } while(td!=prio->ctid);
    
    prio=prio->next;
  } while(prio);
} 
#endif 
    

//! the task switcher
/*! the task switcher saves active context and passes sp to scheduler
    then restores new context from returned sp
*/
void tm_switcher(void);
#ifndef DOXYGEN_SHOULD_SKIP_THIS
__asm__("
.text
.align 1
.globl _tm_switcher
_tm_switcher:
      ; r6 saved by ROM
      ; r0 saved by system timer handler

      mov.w r1,@-r7                           ; save registers
      mov.w r2,@-r7 
      mov.w r3,@-r7 
      mov.w r4,@-r7 
      mov.w r5,@-r7 

      mov.w r7,r0                             ; pass sp

      jsr _tm_scheduler                       ; call scheduler

_tm_switcher_return:    
      mov.w r0,r7                             ; set new sp

      mov.w @r7+,r5
      mov.w @r7+,r4
      mov.w @r7+,r3
      mov.w @r7+,r2
      mov.w @r7+,r1

      ; r0 will be restored by system timer handler
      ; r6 will be restored by ROM

      rts                                     ; return to new task
");
#endif  // DOXYGEN_SHOULD_SKIP_THIS


//! the task scheduler
/*! \param   old_sp current task's current stack pointer
    \return  new task's current stack pointer
  
    actual context switches performed by tm_switcher (assembler wrapper)
*/
size_t *tm_scheduler(size_t *old_sp) {
  tdata_t  *next;                             // next task to execute
  pchain_t *priority;
  wakeup_t tmp;

  priority=ctid->priority;
  switch(ctid->tstate) {
  case T_ZOMBIE:
    if(ctid->next!=ctid) {
      // remove from chain for this priority level
      //

      priority->ctid  =ctid->prev;
      ctid->next->prev=ctid->prev;
      ctid->prev->next=ctid->next;
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
    free(ctid->stack_base);                   // free stack
    free(ctid);                               // free task data

    //
    // FIXME: exit code?
    //

    if ((ctid->tflags & T_KERNEL)==T_KERNEL)
      --nb_system_tasks;

    switch(--nb_tasks) {
    case 1:
#ifdef CONF_TM_DEBUG    
      if((priority_head->ctid->tflags & T_IDLE)==0) {
        // last task is not the idle task
        fatal("ERR00");
      }
#endif // CONF_TM_DEBUG
      // only the idle task remains
      *((priority_head->ctid->sp_save) + SP_RETURN_OFFSET ) = (size_t) &exit;
      priority_head->ctid->tstate=T_SLEEPING;
      break;
    
    case 0:
      // the last task has been removed
      // -> stop switcher, go single tasking
    
      systime_set_switcher(&rom_dummy_handler);
      ctid=&td_single;
    
      return ctid->sp_save;
    }
    break;

  case T_RUNNING:
    ctid->tstate=T_SLEEPING;
    // no break

  case T_WAITING:
    ctid->sp_save=old_sp;
  }


  // find next task willing to run
  //  
  priority=priority_head;
  next=priority->ctid->next;
  while (1) {
    if (next->tstate==T_SLEEPING)
      break;
    
    if (next->tstate==T_WAITING) {
      if ((next->tflags & T_SHUTDOWN) != 0) {
        next->wakeup_data = 0;
        break;
      }
      ctid = next;
      tmp = next->wakeup(next->wakeup_data);
      if (tmp != 0) {
        next->wakeup_data = tmp;
        break;
      }
    }
    
    if(next == priority->ctid) {
      // if we've scanned the whole chain,
      // go to next priority
      
      if(priority->next != NULL) 
        priority = priority->next;
#ifdef CONF_TM_DEBUG        
      else {
        // FIXME: idle task has died
        //        this is a severe error.
        fatal("ERR01");
      }
#else
      else
        priority = priority_head;
#endif
      next=priority->ctid->next;
    } else
      next=next->next;
  }
  ctid=next->priority->ctid=next;             // execute next task
  ctid->tstate=T_RUNNING;

  return ctid->sp_save;
}

//! yield the rest of the current timeslice
/*! (does not speed up the system clock)
*/
extern void yield(void);
#ifndef DOXYGEN_SHOULD_SKIP_THIS
__asm__("
.text
.globl _yield
.align 1
_yield:
      stc     ccr,r0h                ; to fake an IRQ, we have to
      push    r0                     ; store the registers
      orc     #0x80,ccr              ; disable interrupts

      push    r6                     ; store r6

      mov.w   #0x04d4,r0             ; store rom return addr
      push    r0

      push    r0                     ; store r0 (destroyed by call.)

      mov.w   #_systime_tm_return,r0 ; store systime return addr
      push    r0

      jmp     @_tm_switcher          ; call task switcher
");
#endif  // DOXYGEN_SHOULD_SKIP_THIS

//! the idle system task
/*! infinite sleep instruction to conserve power
*/
extern int tm_idle_task(int argc,char **argv) __attribute__ ((noreturn));
#ifndef DOXYGEN_SHOULD_SKIP_THIS
__asm__("
.text
.align 1
_tm_idle_task:
      sleep
      bra _tm_idle_task
");
#endif  // DOXYGEN_SHOULD_SKIP_THIS

#ifdef CONF_VIS
//! the man system task
/*! infinite loop; when program running, update the man (stand/run)
*/
int tm_man_task(int argc, char **argv)
{
  int state=0;

  while (!shutdown_requested()) {
    if(nb_tasks > nb_system_tasks) state ^= 1; else state=0;
    lcd_show(state == 0 ? man_stand : man_run);
#ifndef CONF_LCD_REFRESH
    lcd_refresh();
#endif // CONF_LCD_REFRESH
    msleep(500);
  }
  return 0;
}

#ifdef CONF_BATTERY_INDICATOR
//! the battery system task
/*! updates the battery low indicator when necessary
*/
int tm_battery_task(int argc, char **argv) {
  int bmv;

  while (!shutdown_requested()) {
    bmv=get_battery_mv();

    if(bmv>BATTERY_NORMAL_THRESHOLD_MV)
      dlcd_hide(LCD_BATTERY_X);
    else if(bmv<BATTERY_LOW_THRESHOLD_MV)
      dlcd_show(LCD_BATTERY_X);

    msleep(2000);
  }
  return 0;
}
#endif // CONF_BATTERY_INDICATOR
#endif // CONF_VIS

//! initialize task management
/*! initialize tasking variables and start the system tasks
!*! (called in single tasking mode before task setup.)
*/
void tm_init(void) {
  tdata_t* td_idle;

  // no tasks right now.
  //
  nb_tasks=0;
  nb_system_tasks=0;
  priority_head=NULL;
  INITIALIZE_KERNEL_CRITICAL_SECTION(); 
 
  // the single tasking context
  //
  td_single.tstate=T_RUNNING;
  ctid=&td_single;

  // the idle task is an institution
  //  
  td_idle=(tdata_t*)execi(&tm_idle_task,0,NULL,0,IDLE_STACK_SIZE);
  td_idle->tflags |= T_IDLE;

#ifdef CONF_VIS
  execi(&tm_man_task, 0, NULL, 1, IDLE_STACK_SIZE);

#ifdef CONF_BATTERY_INDICATOR
  execi(&tm_battery_task, 0, NULL, 1, IDLE_STACK_SIZE);
#endif // CONF_BATTERY_INDICATOR
#endif // CONF_VIS

  systime_set_timeslice(TM_DEFAULT_SLICE);
} 


//! start task management 
/*! (called in single tasking mode after task setup)
*/
void tm_start(void) {
  disable_irqs();                               // no interruptions, please

  systime_set_switcher(&tm_switcher);
  yield();                                      // go!

  enable_irqs();                                // restored state would 
                                                // disallow interrupts
}

//! schedule execution of a new task
/*! \param code_start start address of code to execute
    \param argc first argument passed, normally number of strings in argv
    \param argv second argument passed, normally pointer to argument pointers.
    \param priority new task's priority
    \param stack_size stack size for new task
    \return -1: fail, else tid.
    
    will return to caller in any case.
*/
tid_t execi(int (*code_start)(int,char**),int argc, char **argv,
            priority_t priority,size_t stack_size) {
  pchain_t *pchain, *ppchain; // for traversing priority chain
  int freepchain=0;
  
  // get memory
  //
  // task & stack area belong to parent task
  // they aren't freed by mm_reaper()
  //
  // avoid deadlock of memory and task semaphores
  // by preallocation.
  
  tdata_t *td=malloc(sizeof(tdata_t));
  size_t *sp=malloc(stack_size);
  
  // for allocating new priority chain
  pchain_t *newpchain=malloc(sizeof(pchain_t));

  if (td == NULL || sp == NULL || newpchain == NULL)
  {
    free(td);
    free(sp);
    free(newpchain);
    return -1;
  }
  
  td->tflags = 0;
  if ((size_t)code_start < (size_t)&mm_start)
  {
    td->tflags |= T_KERNEL;
    nb_system_tasks++;
  }
  else
    td->tflags |= T_USER;

  td->stack_base=sp;                  // these we know already.

  sp+=(stack_size>>1);                // setup initial stack

  // when main() returns a value, it passes it in r0
  // as r0 is also the register to pass single int arguments by
  // gcc convention, we can just put the address of exit on the stack.

  *(--sp)=(size_t) &exit;

  // we have to construct a stack stub so tm_switcher,
  // systime_handler and the ROM routine can fill the 
  // right values on startup.

  *(--sp)=(size_t) code_start;        // entry point   < these two are for
  *(--sp)=0;                          // ccr           < rte in ROM
  *(--sp)=0;                          // r6      < pop r6 in ROM
  *(--sp)=(size_t)
          &rom_ocia_return;         // ROM return < rts in systime_handler

  *(--sp)=(size_t) argc;              // r0     < pop r0 in systime handler
  *(--sp)=(size_t)              
          &systime_tm_return;       // systime return < rts in tm_switcher

  *(--sp)=(size_t) argv;              // r1..r5 < pop r1..r5 in tm_switcher
  *(--sp)=0;
  *(--sp)=0;
  *(--sp)=0;
  *(--sp)=0;

  td->sp_save=sp;                   // save sp for tm_switcher
  td->tstate=T_SLEEPING;              // task is waiting for execution
  td->parent=ctid;                    // set parent

  ENTER_KERNEL_CRITICAL_SECTION();

  ppchain=NULL;
  for(  pchain = priority_head;
    pchain != NULL && (pchain->priority) > priority;
    ppchain = pchain, pchain = pchain->next
  );
  if(pchain==NULL || pchain->priority!=priority) {
    // make new chain
    //
    newpchain->priority=priority;
    newpchain->ctid=td;

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
    td->prev=td->next=td;
    td->priority=newpchain;
  } else {
    // add at back of queue
    //
    td->priority=pchain;
    td->prev=pchain->ctid->prev;
    td->next=pchain->ctid;
    td->next->prev=td->prev->next=td;
    freepchain=1; // free superfluous pchain.
  }
  nb_tasks++;

  LEAVE_KERNEL_CRITICAL_SECTION();  

  if(freepchain)
    free(newpchain);
  
  return (tid_t) td;                  // tid = (tid_t) &tdata_t_struct
}

//! exit task, returning code
/*! \param code The return code
 
    \todo save return code; temporarily?
    \bug  ignores return code.
*/
void exit(int code) {
  enable_irqs();                                // just in case...
  if (!(ctid->tflags & T_KERNEL))
    mm_reaper();
  ctid->tstate=T_ZOMBIE;
  // Yield till dead
  while(1)
    yield();
}

//! suspend task until wakeup function is non-null
/*! \param wakeup the wakeup function. called in task scheduler context.
    \param data argument passed to wakeup function by scheduler
    \return return value passed on from wakeup, 0 if exits prematurely
*/
wakeup_t wait_event(wakeup_t (*wakeup)(wakeup_t),wakeup_t data) {
  ctid->wakeup     =wakeup;
  ctid->wakeup_data=data;
  ctid->tstate     =T_WAITING;

  yield();

  return ctid->wakeup_data;
}

//! wakeup function for sleep
/*! \param data time to wakeup encoded as a wakeup_t
*/
static wakeup_t tm_sleep_wakeup(wakeup_t data) {
  time_t remaining = ((time_t)data) - get_system_up_time();

  if (((time_t)data) <= get_system_up_time())
  {
    tm_timeslice = TM_DEFAULT_SLICE;
    return -1;
  }

  if (remaining < tm_timeslice)
    tm_timeslice = remaining;

  return 0;
}

//! delay execution allowing other tasks to run.
/*! \param msec sleep duration in milliseconds
    \return number of milliseconds left if interrupted, else 0.
 */
unsigned int msleep(unsigned int msec)
{
#if defined(CONF_TIME) && defined(CONF_TM)
  if (wait_event(&tm_sleep_wakeup, get_system_up_time() + MSECS_TO_TICKS(msec)) == 0)
    return (MSECS_TO_TICKS(msec) - get_system_up_time());
#else
  delay(msec);
#endif
  return 0;
}

//! delay execution allowing other tasks to run.
/*! \param sec sleep duration in seconds
    \return number of seconds left if interrupted, else 0.
 */
unsigned int sleep(unsigned int sec)
{
  return msleep(1000*sec)/1000;
}

//! request that the specified task shutdown as soon as possible.
/*! \param tid id of the task to be shutdown
 */
void shutdown_task(tid_t tid) {
  tdata_t *td=(tdata_t*) tid;
  td->tflags |= T_SHUTDOWN;
}

//! request that tasks with any of the specified flags shutdown.
/*! \param flags T_USER to shutdown user tasks, T_USER | T_KERNEL for all tasks
 */
void shutdown_tasks(tflags_t flags) {
  pchain_t* pchain;
  tdata_t* td;

  ENTER_KERNEL_CRITICAL_SECTION();  
 
  pchain = priority_head;
  while (pchain != NULL) {
    td = pchain->ctid;
    do {
      if ((td->tflags & flags) != 0) {
        // signal shutdown
        //
        td->tflags |= T_SHUTDOWN;
      }
      td = td->next;
    } while (td != pchain->ctid);
    pchain = pchain->next;
  }
 
  LEAVE_KERNEL_CRITICAL_SECTION();
}

//! kill specified task
/*! \param tid must be valid process ID, or undefined behaviour will result!
*/
void kill(tid_t tid) {
  tdata_t *td=(tdata_t*) tid;
  if(td==ctid)
    exit(-1);
  else {
    // when the task is switched to the next time,
    // make it exit immediatlely.

    ENTER_KERNEL_CRITICAL_SECTION(); 

    *( (td->sp_save) + SP_RETURN_OFFSET )=(size_t) &exit;
    td->tstate=T_SLEEPING;    // in case it's waiting.

    LEAVE_KERNEL_CRITICAL_SECTION();
  }
}

//! kill all tasks of prio or lower
/*! \param prio 
*/
void killall(priority_t prio) {
  pchain_t *pchain;
  tdata_t *td;
  tflags_t flags = T_KERNEL | T_IDLE;
  
  if (prio == PRIO_HIGHEST)
    flags = T_IDLE;

  ENTER_KERNEL_CRITICAL_SECTION();

  // find first chain with smaller or equal priority.
  //
  pchain=priority_head;
  while(pchain!=NULL && prio<pchain->priority)
    pchain=pchain->next;

  while(pchain!=NULL) {
    td=pchain->ctid;
    do {
      if((td!=ctid) && ((td->tflags & flags) == 0)) {
        // kill it
        //
        *( (td->sp_save) + SP_RETURN_OFFSET )=(size_t) &exit;
        td->tstate=T_SLEEPING;    // in case it's waiting.
      }
      td=td->next;
    } while(td!=pchain->ctid);
    pchain=pchain->next;
  }  

  LEAVE_KERNEL_CRITICAL_SECTION();
}

#endif // CONF_TM

