/*! \file   setjmp.c
    \brief  Implementation: Non-local goto as described in ANSI C.
    \author Torsten Landschoff <tla@informatik.uni-kiel.de>
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
 *  Contributor(s): Torsten Landschoff <tla@informatik.uni-kiel.de>
 */

#include <setjmp.h>

#ifdef CONF_SETJMP

int     setjmp(jmp_buf env);

#ifndef DOXYGEN_SHOULD_SKIP_THIS
__asm__("\n\
.text\n\
.align 2\n\
.globl _setjmp\n\
_setjmp:\n\
    stc     ccr,r1h         ; remember flags\n\
    mov.w   #12,r2          ; move r0 'behind' jump buffer\n\
    add	    r2,r0           ; for storing with pre-decrement\n\
\n\
    pop     r2              ; get return address\n\
    mov.w   r2,@-r0         ; store return address\n\
    mov.w   r1,@-r0         ; store flags\n\
    mov.w   r7,@-r0         ; store registers r4-r7\n\
    mov.w   r6,@-r0\n\
    mov.w   r5,@-r0\n\
    mov.w   r4,@-r0\n\
    sub.w   r0,r0           ; return value: 0\n\
    jmp     @r2             ; jump to caller\n\
	");
#endif // DOXYGEN_SHOULD_SKIP_THIS


void    longjmp(jmp_buf env, int val);
#ifndef DOXYGEN_SHOULD_SKIP_THIS
__asm__("\n\
.text\n\
.align 2\n\
.globl _longjmp\n\
_longjmp:\n\
    mov.w   @r0+,r4         ; restore registers r4-r7\n\
    mov.w   @r0+,r5\n\
    mov.w   @r0+,r6\n\
    mov.w   @r0+,r7\n\
    mov.w   @r0+,r2         ; get flags\n\
    mov.w   @r0+,r0         ; get return address\n\
    push    r0              ; set up for rts\n\
    mov.w   r1,r0           ; return specified value\n\
    ldc     r2h,ccr         ; restore flags\n\
    rts                     ; return to caller\n\
	");
#endif // DOXYGEN_SHOULD_SKIP_THIS

#endif // CONF_SETJMP
