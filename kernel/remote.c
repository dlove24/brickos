/*! \file   remote.c
    \brief  Implementation: LEGO Infrared Remote Control and data structures
    \author Ross Crawford <rcrawford@csi.com>
*/

/*
 * Copyright (c) 2001 Ross Crawford
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
 */

/*
 *  2002.04.23 - Ted Hess <thess@kitschensync.net>
 *
 *  - Integrated into legOS 0.2.6. Added lr_startup(), lr_shutdown()
 *    Release input buffer while processing keys
 *
 */

#include <remote.h>

#if defined(CONF_LR_HANDLER)

#include <sys/lcd.h>
#include <unistd.h>
#include <lnp/lnp.h>
#include <time.h>
#include <dmotor.h>
#include <dsound.h>
#include <conio.h>
#include <tm.h>

///////////////////////////////////////////////////////////////////////////////
//
// Internal Variables
//
///////////////////////////////////////////////////////////////////////////////

time_t lr_timeoff;         // all keys off if no data received before...
unsigned int lr_curkeys;   // mask of keys currently "ON"

unsigned int lr_data;      // lnp data byte
int lr_dataready = 0;      // lr_data valid?

lr_handler_t lr_handler;   // the user event handler
tid_t lr_tid;     // Button dispatch thread

///////////////////////////////////////////////////////////////////////////////
//
// Functions
//
///////////////////////////////////////////////////////////////////////////////

//! Called from LNP when key data available
void lr_getdata(unsigned int x)
{
  // If previous data hasn't been processed yet, this will be lost
  if (lr_dataready == 0)
  {
    lr_data = x;
    lr_dataready = 1;
  }

  // Reset timeout
  lr_timeoff = get_system_up_time() + LR_TIMEOUT;
}


//! Key state dispatcher (keyup / keydown)
void lr_process(unsigned int lr_keys)
{
  unsigned int keys_on, keys_off, common_keys, k;

  // If keys pressed has changed
  if (lr_keys != lr_curkeys) {

    // Get mask of keys pressed & released since last event
    common_keys = (lr_keys & lr_curkeys);
    keys_on = lr_keys & ~common_keys;
    keys_off = lr_curkeys & ~common_keys;

    // send event to user handler for each change
    if (lr_handler) {
      for (k=1; k; k<<=1) {
        if (keys_on & k)
          lr_handler(LREVT_KEYON,k);
        if (keys_off & k)
          lr_handler(LREVT_KEYOFF,k);
      }
    }

    // store key mask for next time
    lr_curkeys = lr_keys;
  }
  return;
}
 
wakeup_t lr_waitdata(wakeup_t data)
{
  // if time runs out, fake "all keys off"
  if (get_system_up_time() > lr_timeoff && lr_curkeys != 0) {
    lr_data = 0;
    lr_dataready = 1;
  }

  // tell lr_thread whether there's any data available
  return lr_dataready;
}

//! lr_thread just sits waiting for data, processing it as it arrives
int lr_thread(int argc, char *argv[]) {
  unsigned int lr_keys;
  while(!shutdown_requested()) {
    if (wait_event(&lr_waitdata, 0) != 0) {
      // Snatch input before calling user handler
      lr_keys = lr_data;
      // Have local copy of input data, allow buffer to refill
      lr_dataready = 0;
      // Call user handler
      lr_process(lr_keys);
    }
  }
  return 0;
}

//! Init remote key dispatcher
void lr_init()
{
  lnp_remote_set_handler(lr_getdata);
  return;
}


//! Startup lr_thread and init protocol handler.
void lr_startup()
{
  // start with all keys off, set initial timeout, clear user handler
  lr_curkeys = 0;
  lr_timeoff = get_system_up_time() + LR_TIMEOUT;
  lr_handler = NULL;

  // Start watcher thread, then tell lnp where we want remote data to go
  lr_tid = execi(&lr_thread,0,0,PRIO_HIGHEST,DEFAULT_STACK_SIZE);
  lr_init();

  return;
}

//! Shutdown protocol handler and terminate thread?
void lr_shutdown()
{
  // Disconnect protocol handler
  lnp_remote_set_handler(LNP_DUMMY_REMOTE);
  lr_set_handler(LR_DUMMY_HANDLER);

  // terminate thread
  kill(lr_tid);

  return;
}

#endif  // CONF_LR_HANDLER
