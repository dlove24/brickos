/*! \file   mulhi3.c
    \brief  16 bit multiplication routine
    \author Markus L. Noga <markus@noga.de>
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
 */

//! 16 bit signed multiplication */
/*! \param  a multiplicand
    \param  b multiplicand
    \return product
*/
int __mulhi3(int a,int b);

__asm__ ("
.section .text
.global ___mulhi3
___mulhi3:
      ; param   r0,r1
      ; return  r0
      ; clobber r2
    
      mov.w    r0,r2
      mulxu.b  r1h,r2
      mov.b    r0h,r2h
      mulxu.b  r1l,r0
      add.b    r2l,r0h
      mulxu.b  r2h,r1
      add.b    r1l,r0h
      rts
");
