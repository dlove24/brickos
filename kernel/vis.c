/*! \file   vis.c
    \brief  Implementation: visualization of BrickOS state
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

#include <sys/vis.h>

#ifdef CONF_VIS

#include <conio.h>
#include <sys/irq.h>
#include <dsensor.h>
#include <dmotor.h>
#include <sys/program.h>
#include <sys/tm.h>

///////////////////////////////////////////////////////////////////////////////
//
// Variables
//
///////////////////////////////////////////////////////////////////////////////

unsigned char vis_refresh_counter = 0;
unsigned char vis_refresh_period = 50;

///////////////////////////////////////////////////////////////////////////////
//
// Functions
//
///////////////////////////////////////////////////////////////////////////////

#ifdef CONF_RCX_COMPILER
void vis_handler(void) {
#else
HANDLER_WRAPPER("vis_handler","vis_core");
void vis_core(void) {
#endif
#ifdef CONF_DSENSOR
  bit_iload(AD_C_H,0x7);
  dlcd_store(LCD_S1_ACTIVE);

  bit_iload(AD_B_H,0x7);
  dlcd_store(LCD_S2_ACTIVE);

  bit_iload(AD_A_H,0x7);
  dlcd_store(LCD_S3_ACTIVE);
#endif

#ifdef CONF_PROGRAM
  if(nb_tasks <= nb_system_tasks) {    	      	  // show only while program not running.
    if(program_valid(cprog))
      cputc_hex_0(cprog+1);
    else
      cputc_0('-');
  }
#endif
}

#endif  // CONF_VIS
