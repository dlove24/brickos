/*! \file include/atomic.h
    \brief Interface: critical section management
    \author Henner Zeller <H.Zeller@acm.org>

    Defines types and functions to implement atomic counters.
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
 *                  Henner Zeller     <H.Zeller@acm.org>
 */
#ifndef __atomic_h__
#define __atomic_h__

#include <config.h>

#ifdef CONF_ATOMIC

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * The data type that allows for atomic count operations.
 * @see atomic_inc
 */
typedef volatile unsigned char atomic_t;

/**
 * decrement atomic counter without interruption.
 * locks interrupts except NMI, decrements count
 * then restores interrupts.
 * @param counter the counter resource to be decremented
 * @see atomic_inc
 */
extern void atomic_dec(atomic_t* counter);

/**
 * increment atomic counter without interruption.
 * locks interrupts except NMI, increments count
 * then restores interrupts.
 * @param counter the counter resource to be incremented
 * @see atomic_dec
 */
extern void atomic_inc(atomic_t* counter);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* CONF_ATOMIC */

#endif /* __critsec_h__ */
