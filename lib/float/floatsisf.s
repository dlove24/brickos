/*
 *  floatsisf.s
 *
 *  Signed long to floating point conversion, single precision:
 *     r0r1 = (float) long r0r1
 *  Unsigned long to floating point conversion, single precision:
 *     r0r1 = (float) unsigned long r0r1
 *
 *  GCC does not directly invoke the unsigned long conversion function
 *  Instead, it wraps the signed long function with a good amount of extra code
 *  You can save space by calling the latter conversion function yourself
 *  If you want to use it, you should invoke it yourself, as a function
 *  The declaration is: extern float __ufloatsisf (unsigned long value);
 *
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
 *  The Original Code is Librcx floating point code, released May 27, 1999.
 *
 *  The Initial Developer of the Original Code is Kekoa Proudfoot.
 *  Portions created by Kekoa Proudfoot are Copyright (C) 1999
 *  Kekoa Proudfoot. All Rights Reserved.
 *
 *  Contributor(s): Kekoa Proudfoot <kekoa@graphics.stanford.edu>
 */

; possible optimizations
;  - integrate msb test and sticky shift with fixoverflow in joinsf
;       - add a bor #7,r5h to see if four bit is set
;  - factor out stickyshift function, rewrite fixoverflow in joinsf to call it
;       - affects this, since we can then call stickyshift right
;  - if ufloatsisf removed, msb test should set mantissa to 7fffffff
;       - also, msb test should be moved earlier to inside negative input test

    .section .text

;;
;; function: ufloatsisf
;; input: unsigned long in r0r1
;; output: float in r0r1
;;

    .global ___ufloatsisf

___ufloatsisf:

    ; Save registers (assume r2 and r3 saved by caller)

    push    r4
    push    r5
    mov.w   r6,r2               ; we need r6 specifically, but do not need r2

    ; Jump into floatsisf

    bra     ufloatentry



;;
;; function: floatsisf
;; input: signed long in r0r1
;; output: float in r0r1
;;

    .global ___floatsisf

___floatsisf:

    ; Save registers (assume r2 and r3 saved by caller)

    push    r4
    push    r5
    mov.w   r6,r2               ; we need r6 specifically, but do not need r2

    ; Is input negative?

    btst    #7,r0h              ; is r0r1 < 0?
    beq     else_0              ; equal indicates false

        ; Set sign to negative

        mov.b   #0x80,r3l       ; set sign (r3l) to 0x80 to indicate negative

        ; Store -input to mantissa

        sub.w   r5,r5           ; zero destination
        sub.w   r6,r6

        sub.w   r1,r6           ; subtract input
        subx.b  r0l,r5l
        subx.b  r0h,r5h

        bra     endif_0

    else_0:

ufloatentry:

        ; Set sign to positive

        sub.b   r3l,r3l         ; set sign (r3l) to zero to indicate positive

        ; Store input to mantissa

        mov.w   r0,r5           ; set mantissa
        mov.w   r1,r6

    endif_0:

    ; Set exponent

    mov.w   #156,r4             ; set exponent to 29 + bias, bias is 127

    ; Is msb set?

    btst    #7,r5h              ; is msb set?
    beq     else_1              ; zero indicates false

        ; Mantissa overflow, so shift right one place, maintaining sticky bit

        shlr.b  r5h             ; shift mantissa right 1 place
        rotxr.b r5l
        rotxr.b r6h
        rotxr.b r6l             ; last shift places old sticky bit in carry

        bor     #0,r6l          ; or lsb with old sticky bit
        bst     #0,r6l          ; store new sticky bit

        ; Increase exponent

        adds.w  #1,r4           ; add one to exponent

        bra     endif_1

    else_1:

        ; Is mantissa non-zero?

        mov.w   r5,r5           ; check upper word
        bne     if_2            ; not equal to zero indicates true
        mov.w   r6,r6           ; check lower word
        beq     endif_2         ; equal to zero indicates false

            if_2:

            ; Is the two bit (40000000) clear?

            btst    #6,r5h      ; check 40000000 bit
            bne     endif_3     ; non-zero indicates false (bit set)

                ; Normalize

                jsr  ___normalizesf

            endif_3:

        endif_2:

    endif_1:

    ; Join

    jsr  ___joinsf

return:

    ; Restore registers

    mov.w   r2,r6
    pop     r5
    pop     r4

    rts
