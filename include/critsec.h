/*! \file include/critsec.h
    \brief Interface: critical section management
    \author Joseph A. Woolley <jawoolley@users.sourceforge.net>

    Defines types and functions to implement critical sections.
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
 *  Contributor(s): Joseph A. Woolley <jawoolley@users.sourceforge.net
 */

#if !defined(__critsec_h__)
#define __critsec_h__

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

#include <config.h>

#if defined(CONF_TM) && defined(CONF_CRITICAL_SECTIONS)
#include <sys/tm.h>

#ifndef DOXYGEN_SHOULD_SKIP_INTERNALS
//! critical section data structure
/*! tracks current count (level of nesting)
    and the task that is in the critical section(s)
    count will be zero when no tasks are within
    a critical section.
 */
struct critsec {
  unsigned char count;
  tdata_t* task;
};
#endif //  DOXYGEN_SHOULD_SKIP_INTERNALS

//! critical section type definition
typedef struct critsec critsec_t;

//! decrement counter without interruption
/*! locks interrupts except NMI, decrements count
    then restores interrupts.
    \param counter the counter resource to be decremented
    \return always 0 (currently)
    \sa locked_increment
 */
extern int locked_decrement(unsigned char* counter);

//! wakeup when critical section is available
/*! wakeup function used to sleep a task until a critical
    section is available.  called while processing an
    interrupt, so interrupts are already disabled.
    \return 0 to continue waiting, non-zero to wakeup task.
    \sa enter_critical_section
 */ 
extern wakeup_t wait_critical_section(wakeup_t data);

//! initialize critical section
/*! sets count field of critical section to zero
    \param cs pointer to critical section (critsec_t)
    \return always 0
    \sa enter_critical_section
    \sa leave_critical_section
    \sa destroy_critical_section
 */
#define initialize_critical_section(cs) (cs)->count=0

//! enter critical section
/*! block other tasks if they attempt to enter a
    region of code protected by the same critical section.
    \param cs pointer to critical section (critsec_t)
    \return 1 if successful, 0 if failure
    \sa initialize_critical_section
    \sa leave_critical_section
    \sa destroy_critical_section
 */
extern int enter_critical_section(critsec_t* cs);

//! leave critical section
/*! allow other tasks to enter critical regions
    of code protected by this critical section.
    \param cs pointer to critical section (critsec_t)
    \return results of locked_decrement (always 0)
    \sa initialize_critical_section
    \sa enter_critical_section
    \sa destroy_critical_section
 */
#define leave_critical_section(cs) locked_decrement(&(cs)->count)

//! destroy critical section (does nothing)
/*! currently there are no resources that are dynamically
    allocated.
    \param cs pointer to critical section (critsec_t)
    \sa initialize_critical_section
    \sa enter_critical_section
    \sa leave_critical_section
 */
#define destroy_critical_section(cs)

#endif // CONF_TM && CONF_CRITICAL_SECTIONS

#if defined(__cplusplus)
}
#endif // __cplusplus

#endif // __critsec_h__
