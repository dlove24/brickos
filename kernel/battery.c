/*! \file   battery.c
    \brief  Inplementation: battery functions and low battery LCD indicator handling
    \author Paolo Masetti <paolo.masetti@itlug.org>
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
 *  Contributor(s): Markus L. Noga <markus@noga.de>
 *        Paolo Masetti  <paolo.masetti@itlug.org>
 */

#include <sys/irq.h>
#include <dsensor.h>
#include <dlcd.h>

#include <sys/battery.h>

///////////////////////////////////////////////////////////////////////
//
// Global Variables
//
///////////////////////////////////////////////////////////////////////
#ifdef CONF_BATTERY_INDICATOR
unsigned int battery_refresh_counter = 0; //!< counter for lcd refresh in ms
unsigned int battery_refresh_period  = 2000;  //!< LCD refresh period in ms
#endif

///////////////////////////////////////////////////////////////////////
//
// Functions
//
///////////////////////////////////////////////////////////////////////

//! return the approximate battery voltage (in milliVolts)
#if defined(CONF_DSENSOR)
int get_battery_mv()
{
    long b = ds_scale(BATTERY) * 0xABD4L;
    return (int)(b / 0x618L);
}
#endif // CONF_DSENSOR

#ifdef CONF_BATTERY_INDICATOR
// battery indicator handed by kernel task when CONF_TM
#ifndef CONF_TM
//! battery indicator handler, called from system timer interrupt
#ifdef CONF_RCX_COMPILER
void battery_refresh(void) {
#else // CONF_RCX_COMPILER
HANDLER_WRAPPER("battery_refresh","battery_refresh_core");
void battery_refresh_core(void) {
#endif // CONF_RCX_COMPILER
    int bmv = get_battery_mv();

    if (bmv > BATTERY_NORMAL_THRESHOLD_MV)
  dlcd_hide(LCD_BATTERY_X);
    else if (bmv > BATTERY_LOW_THRESHOLD_MV)
  dlcd_show(LCD_BATTERY_X);
}
#endif // CONF_TM
#endif // CONF_BATTERY_INDICATOR
