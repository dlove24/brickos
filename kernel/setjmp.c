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
__asm__("
.text
.align 2
.globl _setjmp
_setjmp:
    stc     ccr,r1h         ; remember flags
    mov.w   #12,r2          ; move r0 'behind' jump buffer
    add	    r2,r0           ; for storing with pre-decrement

    pop     r2              ; get return address
    mov.w   r2,@-r0         ; store return address
    mov.w   r1,@-r0         ; store flags
    mov.w   r7,@-r0         ; store registers r4-r7
    mov.w   r6,@-r0
    mov.w   r5,@-r0
    mov.w   r4,@-r0
    sub.w   r0,r0           ; return value: 0
    jmp     @r2             ; jump to caller
	");
#endif // DOXYGEN_SHOULD_SKIP_THIS


void    longjmp(jmp_buf env, int val);
#ifndef DOXYGEN_SHOULD_SKIP_THIS
__asm__("
.text
.align 2
.globl _longjmp
_longjmp:
    mov.w   @r0+,r4         ; restore registers r4-r7
    mov.w   @r0+,r5
    mov.w   @r0+,r6
    mov.w   @r0+,r7
    mov.w   @r0+,r2         ; get flags
    mov.w   @r0+,r0         ; get return address
    push    r0              ; set up for rts
    mov.w   r1,r0           ; return specified value
    ldc     r2h,ccr         ; restore flags
    rts                     ; return to caller
	");
#endif // DOXYGEN_SHOULD_SKIP_THIS

#endif // CONF_SETJMP
