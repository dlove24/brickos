/*! \file include/config.h
		\brief	kernel configuration file
		\author Markus L. Noga <markus@noga.de>
 */

/*
 *	The contents of this file are subject to the Mozilla Public License
 *	Version 1.0 (the "License"); you may not use this file except in
 *	compliance with the License. You may obtain a copy of the License at
 *	http://www.mozilla.org/MPL/
 *
 *	Software distributed under the License is distributed on an "AS IS"
 *	basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 *	License for the specific language governing rights and limitations
 *	under the License.
 *
 *	The Original Code is legOS code, released October 17, 1999.
 *
 *	The Initial Developer of the Original Code is Markus L. Noga.
 *	Portions created by Markus L. Noga are Copyright (C) 1999
 *	Markus L. Noga. All Rights Reserved.
 *
 *	Contributor(s): Markus L. Noga <markus@noga.de>
 */

#ifndef __config_h__
#define __config_h__

// compilation environment
//
// #define CONF_RCX_COMPILER							//!< a special RCX compiler is used.

// core system services
//
#define CONF_TIME												//!< system time
#define CONF_MM													//!< memory management
#define CONF_TM													//!< task management
#define CONF_AUTOSHUTOFF								//!< power down after x min of inactivity
//#define CONF_TM_DEBUG										//!< view key shows current instruction pointer
#define CONF_SEMAPHORES									//!< POSIX semaphores
#define CONF_PROGRAM										//!< dynamic program loading support
#define CONF_VIS												//!< generic visualization.
//#define CONF_ROM_MEMCPY									//!< Use the ROM memcpy routine

// networking services
//
#define CONF_LNP												//!< link networking protocol
// #define CONF_LNP_FAST									//!< enable 4800 bps LNP
// Can override with compile-time option
#if !defined(CONF_LNP_HOSTADDR)
#define CONF_LNP_HOSTADDR 0 						//!< LNP host address
#endif

// 16 nodes x 16 ports (affects size of lnp_addressing_handler[] table)
#define CONF_LNP_HOSTMASK 0xf0					//!< LNP host mask

// remote control services
//
#define CONF_RCX_PROTOCOL								//!< RCX protocol handler
#define CONF_LR_HANDLER									//!< remote control keys handler service
#define CONF_RCX_MESSAGE								//!< standard firmware message service

// drivers
//
#define CONF_DKEY												//!< debounced key driver
#define CONF_BATTERY_INDICATOR					//!< automatic update of lcd battery indicator
#define CONF_LCD_REFRESH								//!< automatic display updates
#define CONF_CONIO											//!< console
#define CONF_ASCII											//!< ascii console
#define CONF_DSOUND											//!< direct sound
#define CONF_DMOTOR											//!< direct motor
// #define CONF_DMOTOR_HOLD								//!< experimental: use hold mode PWM instead of coast mode.
#define CONF_DSENSOR										//!< direct sensor
#define CONF_DSENSOR_ROTATION						//!< rotation sensor
//#define CONF_DSENSOR_VELOCITY						//!< rotation sensor velocity
//#define CONF_DSENSOR_MUX         //!< sensor multiplexor
//#define CONF_DSENSOR_SWMUX      //!< techno-stuff swmux sensor

// dependencies
//
#if defined(CONF_ASCII) && !defined(CONF_CONIO)
#error "Ascii needs console IO"
#endif

#if defined(CONF_DKEY) && !defined(CONF_TIME)
#error "Key debouncing needs system time."
#endif

#if defined(CONF_TM) && !defined(CONF_TIME)
#error "Task management needs system time."
#endif

#if defined(CONF_TM) && !defined(CONF_MM)
#error "Task management needs memory management."
#endif

#if defined(CONF_MM) && defined(CONF_TM) && !defined(CONF_SEMAPHORES)
#error "Tasksafe memory management needs semaphores."
#endif

#if defined(CONF_LNP) && defined(CONF_TM) && !defined(CONF_SEMAPHORES)
#error "Tasksafe networking needs semaphores."
#endif

#if defined(CONF_RCX_PROTOCOL) && !defined(CONF_LNP)
#error "RCX protocol needs networking."
#endif

#if defined(CONF_LR_HANDLER) && !defined(CONF_RCX_PROTOCOL)
#error "Remote control handler needs remote control protocol."
#endif

#if defined(CONF_RCX_MESSAGE) && !defined(CONF_LNP)
#error "Standard firmware message needs networking."
#endif

#if defined(CONF_LR_HANDLER) && !defined(CONF_TM)
#error "Remote support needs task managment"
#endif

#if defined(CONF_PROGRAM) && (!defined(CONF_TM) || !defined(CONF_LNP) || !defined(CONF_DKEY) || !defined(CONF_ASCII))
#error "Program support needs task management, networking, key
debouncing, and ASCII."
#endif

#if defined(CONF_DSENSOR_ROTATION) && !defined(CONF_DSENSOR)
#error "Rotation sensor needs general sensor code."
#endif

#if defined(CONF_DSENSOR_VELOCITY) && !defined(CONF_DSENSOR_ROTATION)
#error "Velocity sensor needs rotation sensor code."
#endif

#endif // __config_h__
