/*! \file   timeout.c
    \brief  Implementation: Powerdown Timer Routines
    \author Rossz Vamos-Wentworth <rossw@jps.net>
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
 *        Rossz Vámos-Wentworth <rossw@jps.net>
 */

#include <sys/irq.h>
#include <sys/tm.h>
#include <sys/timeout.h>

#ifdef CONF_AUTOSHUTOFF
volatile unsigned int auto_shutoff_counter = 0;   //<! current count - used by the system timer
unsigned int auto_shutoff_period = 500;  //<! milliseconds between shutoff checks
unsigned int auto_shutoff_secs = DEFAULT_SHUTOFF_TIME;  //<! seconds of idle to auto shutoff
volatile unsigned int auto_shutoff_elapsed = 0;   //<! idle seconds elapsed
volatile unsigned int idle_powerdown = 0;   //<! true if a auto-shutoff should occur

void shutoff_restart(void) {
  auto_shutoff_elapsed=0;
  auto_shutoff_counter=auto_shutoff_period;
}

void shutoff_init(void) {
//  auto_shutoff_secs = DEFAULT_SHUTOFF_TIME;
  auto_shutoff_elapsed=0;
  idle_powerdown = 0;
}

#ifdef CONF_RCX_COMPILER
void autoshutoff_check(void) {
#else
HANDLER_WRAPPER("autoshutoff_check","autoshutoff_check_core");
void autoshutoff_check_core(void) {
#endif
#ifdef CONF_TM
  if (nb_tasks <= nb_system_tasks) {
#endif // CONF_TM
    auto_shutoff_elapsed++;
    if (auto_shutoff_elapsed > auto_shutoff_secs)
      idle_powerdown = 1;
#ifdef CONF_TM
  }
   else
    shutoff_restart();
#endif // CONF_TM
}

#endif
