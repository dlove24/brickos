/*! \file include/sys/critsec.h
    \brief Interface: kernel level critical sections
    \author Joseph A. Woolley <jawoolley@users.sourceforge.net>

    Defines types and functions used for kernel level critical sections.
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
 *  Contributor(s): Joseph A. Woolley <jawoolley@users.sourceforge.net> 
 */

#if !defined(__sys_critsec_h__)
#define __sys_critsec_h__

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

#include <config.h>
#include "../critsec.h"

#if defined(CONF_TM)
extern volatile unsigned char kernel_critsec_count;
extern int locked_increment(unsigned char* count);
extern int locked_decrement(unsigned char* count);
#define INITIALIZE_KERNEL_CRITICAL_SECTION() kernel_critsec_count=0
#define ENTER_KERNEL_CRITICAL_SECTION() locked_increment(&kernel_critsec_count)
#define LEAVE_KERNEL_CRITICAL_SECTION() locked_decrement(&kernel_critsec_count)
#define DESTROY_KERNEL_CRITICAL_SECTION()
#else // CONF_TM
#define INITIALIZE_KERNEL_CRITICAL_SECTION()
#define ENTER_KERNEL_CRITICAL_SECTION()
#define LEAVE_KERNEL_CRITICAL_SECTION()
#define DESTROY_KERNEL_CRITICAL_SECTION()
#endif // CONF_TM

#if defined(__cplusplus)
}
#endif // __cplusplus

#endif // __sys_critsec_h__

