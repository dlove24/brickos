/*! \file   include/remote.h
    \brief  Interface: LEGO Infrared Remote Control function and constants
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
 *
 */

#ifndef _REMOTE_H
#define _REMOTE_H

#ifdef  __cplusplus
extern "C" {
#endif

#include <config.h>

#ifdef CONF_LR_HANDLER


#define LR_TIMEOUT 100 // msec

#define LRKEY_M1 0x1
#define LRKEY_M2 0x2
#define LRKEY_M3 0x4
#define LRKEY_A1 0x8
#define LRKEY_B1 0x10
#define LRKEY_C1 0x20
#define LRKEY_A2 0x40
#define LRKEY_B2 0x80
#define LRKEY_C2 0x100
#define LRKEY_P1 0x200
#define LRKEY_P2 0x400
#define LRKEY_P3 0x800
#define LRKEY_P4 0x1000
#define LRKEY_P5 0x2000
#define LRKEY_STOP 0x4000
#define LRKEY_BEEP 0x8000

enum {
  LREVT_KEYON = 1,
  LREVT_KEYOFF = 2
} EventType;

//! the remote key handler type
/*! arguments are (event_type, key).
    returns 0 if not processed, otherwise non-zero.
 */
typedef int (*lr_handler_t) (unsigned int, unsigned int);

//! remote handler.
extern lr_handler_t lr_handler;

void lr_startup();
void lr_init();
void lr_shutdown();

extern inline void lr_set_handler(lr_handler_t handler) {
  lr_handler = handler;
}

//! dummy remote event handler
#define LR_DUMMY_HANDLER ((lr_handler_t)0)

#endif	// CONF_TM

#ifdef  __cplusplus
}
#endif

#endif
