/*! \file   include/config.h
    \brief  kernel configuration file
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

#ifndef __config_h__
#define __config_h__

// compilation environment
//
//#define CONF_RCX_COMPILER //!< a special RCX compiler is used.
#define CONF_HOST

// core system services
//
#define CONF_TIME   //!< system time
#define CONF_MM     //!< memory management
#define CONF_TM     //!< task management
#define CONF_TM_VIS   //!< display task statuus
#define CONF_TM_DEBUG           //!< view key shows current instruction pointer
#define CONF_SEMAPHORES         //!< POSIX semaphores
#define CONF_PROGRAM          //!< dynamic program loading support

// networking services
//
#define CONF_LNP    //!< link networking protocol
//#define CONF_LNP_VIS    //!< display LNP activity
//#define CONF_LNP_FAST        //!< enable 4800 bps LNP
#define CONF_LNP_HOSTADDR 0x8 //!< LNP host address
#define CONF_LNP_HOSTMASK 0xf0  //!< LNP host mask

// drivers
//
//#define CONF_DKEY   //!< debounced key driver
//#define CONF_LCD_REFRESH  //!< automatic display updates
//#define CONF_CONIO    //!< console
//#define CONF_ASCII    //!< ascii console
//#define CONF_DSOUND   //!< direct sound
//#define CONF_DMOTOR             //!< direct motor 
//#define CONF_DSENSOR            //!< direct sensor
//#define CONF_DSENSOR_ROTATION //!< rotation sensor
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

#if defined(CONF_PROGRAM) && (!defined(CONF_TM) || !defined(CONF_LNP))
#error "Programm support needs task management and networking"
#endif

#if defined(CONF_DSENSOR_ROTATION) && !defined(CONF_DSENSOR)
#error "Rotation sensor needs general sensor code."
#endif

#endif // __config_h__
