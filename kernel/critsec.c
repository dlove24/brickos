
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
__asm__("\n\
.text\n\
.global _locked_check_and_increment\n\
        _locked_check_and_increment:\n\
          push.w r4\n\
          stc    ccr, r4h\n\
          orc    #0x80, ccr\n\
          mov.b  @r0, r4l\n\
          beq    lci_get_lock\n\
\n\
          push.w r2\n\
          push.w r3\n\
          mov.w  @_ctid, r2\n\
          mov.w  @r1, r3\n\
          sub.w  r3, r2\n\
          bne    lci_cant_lock\n\
\n\
          pop.w  r3\n\
          pop.w  r2\n\
          bra    lci_get_lock\n\
\n\
        lci_cant_lock:\n\
          pop.w  r3\n\
          pop.w  r2\n\
          mov.w  #0xffff, r0\n\
          bra    lci_done\n\
\n\
        lci_get_lock:\n\
          inc    r4l\n\
          mov.b  r4l, @r0\n\
          mov.w  @_ctid, r0 \n\
          mov.w  r0, @r1 \n\
          sub.w  r0, r0\n\
\n\
        lci_done:\n\
          ldc    r4h, ccr\n\
          pop.w  r4\n\
          rts\n\
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

