/*
 *  cmpsi2.c
 *
 *  Implementation of cmpsi2, a 32-bit signed compare: r0r1 <=> r2r3
 *  Returns -1, 0, or 1, which might not be correct.
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
 *  The Original Code is Librcx code, released February 9, 1999.
 *
 *  The Initial Developer of the Original Code is Kekoa Proudfoot.
 *  Portions created by Kekoa Proudfoot are Copyright (C) 1999
 *  Kekoa Proudfoot. All Rights Reserved.
 *
 *  Contributor(s): Kekoa Proudfoot <kekoa@graphics.stanford.edu>
 */

__asm__	("
	.section .text

	.global	___cmpsi2

___cmpsi2:

	sub.w	r3,r1
	subx.b	r2l,r0l
	subx.b	r2h,r0h

	blt		else_0

		beq		else_1

			; First operand greater than second operand

			mov.w	#1,r0
			rts

		else_1:

			; First operand equal to second operand

			sub.w	r0,r0
			rts

		endif_1:

	else_0:

		; First operand less than second operand

		mov.w	#-1,r0
		rts

	endif_0:

	; Not reached
");
