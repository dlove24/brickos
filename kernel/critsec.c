
#include "config.h"
#include <sys/critsec.h>
#include <critsec.h>
#include <unistd.h>

#if defined(CONF_TM)
#include <sys/tm.h>
#include <tm.h>

//! critical section counter for kernel/task manager
/*! when the critical section counter is Zero,
    task swapping is allowed.  when greater than
    zero, task swapping is not allowed. This is checked in
    the task_switch_handler() in systime.c
 */
atomic_t kernel_critsec_count;

#if defined(CONF_CRITICAL_SECTIONS)
//! check and increment counter, without interruptions
/*! increments counter if count is already zero, or
    if this is the same task as the one that made it one.

    used to enter a normal critical section.

    \param counter pointer to counter to be incremented
    \param tid pointer to pointer to receive task pointer
           of the task that has locked the resource
    \return 0xffff if failure, 0 if successful
    \sa locked_decrement
 */
int locked_check_and_increment(atomic_t* counter, tdata_t** tid);
__asm__("
.text
.global _locked_check_and_increment
        _locked_check_and_increment:
          push.w r4
          stc    ccr, r4h
          orc    #0x80, ccr
          mov.b  @r0, r4l
          beq    lci_get_lock

          push.w r2
          push.w r3
          mov.w  @_ctid, r2
          mov.w  @r1, r3
          sub.w  r3, r2
          bne    lci_cant_lock

          pop.w  r3
          pop.w  r2
          bra    lci_get_lock

        lci_cant_lock:
          pop.w  r3
          pop.w  r2
          mov.w  #0xffff, r0
          bra    lci_done

        lci_get_lock:
          inc    r4l
          mov.b  r4l, @r0
          mov.w  @_ctid, r0 
          mov.w  r0, @r1 
          sub.w  r0, r0

        lci_done:
          ldc    r4h, ccr
          pop.w  r4
          rts
        ");

//! wakeup when critical section is available
/*! wakeup function used to detect when a critical
    section is available.  called while processing
    an interrupt, so interrupts are already disabled.
 */
wakeup_t wait_critical_section(wakeup_t data) {
  critsec_t* cs = (critsec_t*)((unsigned)data);
  if (locked_check_and_increment(&cs->count, &cs->task) == 0xffff)
    return 0;
  else
    return 1;
}

//! lock a critical section, or wait until it is available.
/*! check and lock critical section if it is available;
    otherwise, wait until it is available, then lock it.

    \param cs pointer to critical section (critsec_t)
    \return 1 if successful, 0 upon failure
    \sa initialize_critical_section
    \sa leave_critical_section
    \sa destroy_critical_section
 */
int enter_critical_section(critsec_t* cs) {
  if (locked_check_and_increment(&cs->count, &cs->task) == 0xffff)
    return wait_event(&wait_critical_section, (wakeup_t)((unsigned)cs));
  return 1;
}
#endif // CONF_CRITICAL_SECTIONS
#endif // CONF_TM

