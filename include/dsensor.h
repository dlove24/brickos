/*! \file   include/dsensor.h
    \brief  direct sensor access
    \author Markus L. Noga <markus@noga.de>
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
 */

/*
 *   2000.04.30 - Paolo Masetti <paolo.masetti@itlug.org>
 *
 *	- Some typecast & ()s in macros to avoid strange effects
 *        using them...
 *
 *  2000.09.06 - Jochen Hoenicke <jochen@gnu.org>
 *
 *	- Added velocity calculation for rotation sensor.
 */


#ifndef	__dsensor_h__
#define __dsensor_h__

#ifdef  __cplusplus
extern "C" {
#endif

#include <config.h>

#ifdef CONF_DSENSOR

#include <sys/h8.h>
#include <sys/bitops.h>

///////////////////////////////////////////////////////////////////////
//
// Definitions
//
///////////////////////////////////////////////////////////////////////

//
// the raw sensors
//
#define SENSOR_1	AD_C	//!< Sensor 1
#define SENSOR_2	AD_B	//!< Sensor 2
#define SENSOR_3	AD_A	//!< Sensor 3
#define BATTERY		AD_D	//!< Battery sensor

//
// active light sensor: estimated raw values
//
#define LIGHT_RAW_BLACK 0xffc0	//!< active light sensor raw black value
#define LIGHT_RAW_WHITE 0x5080	//!< active light sensor raw white value


//
// convert raw values to 0 (dark) .. LIGHT_MAX (bright)
// roughly 0-100.
//
#define LIGHT(a)    (147 - ds_scale(a)/7)	//!< map light sensor to 0..LIGHT_MAX
#define LIGHT_MAX   LIGHT(LIGHT_RAW_WHITE)	//!< maximum decoded value

//
// processed active light sensor
//
#define LIGHT_1     LIGHT(SENSOR_1)
#define LIGHT_2     LIGHT(SENSOR_2)
#define LIGHT_3     LIGHT(SENSOR_3)

#ifdef CONF_DSENSOR_ROTATION
//
// processed rotation sensor
//
#define ROTATION_1  (ds_rotations[2])
#define ROTATION_2  (ds_rotations[1])
#define ROTATION_3  (ds_rotations[0])
#endif

#ifdef CONF_DSENSOR_VELOCITY
//
// processed velocity sensor
//
#define VELOCITY_1  (ds_velocities[2])
#define VELOCITY_2  (ds_velocities[1])
#define VELOCITY_3  (ds_velocities[0])
#endif


//! Convert raw data to touch sensor (0: off, else pressed)
#define TOUCH(a)    ((unsigned int)(a) < 0x8000)

//  Processed touch sensors
//
#define TOUCH_1     TOUCH(SENSOR_1)
#define TOUCH_2     TOUCH(SENSOR_2)
#define TOUCH_3     TOUCH(SENSOR_3)


#define ds_scale(x)   ((unsigned int)(x)>>6)
#define ds_unscale(x) ((unsigned int)(x)<<6)

///////////////////////////////////////////////////////////////////////
//
// Variables
//
///////////////////////////////////////////////////////////////////////

//
// don't manipulate directly unless you know what you're doing!
//

extern unsigned char ds_activation;	//!< activation bitmask

#ifdef CONF_DSENSOR_ROTATION
extern unsigned char ds_rotation;	//!< rotation   bitmask

extern volatile int ds_rotations[3];	//!< rotational position

#endif
#ifdef CONF_DSENSOR_VELOCITY
extern volatile int ds_velocities[3];	//!< rotational velocity
#endif

///////////////////////////////////////////////////////////////////////
//
// Functions
//
///////////////////////////////////////////////////////////////////////

//! set sensor mode to active (light sensor emits light, rotation works)
/*! \param  sensor: &SENSOR_1,&SENSOR_2,&SENSOR_3
*/
extern inline void ds_active(volatile unsigned *sensor)
{
  if (sensor == &SENSOR_3)
    bit_set(&ds_activation, 0);
  else if (sensor == &SENSOR_2)
    bit_set(&ds_activation, 1);
  else if (sensor == &SENSOR_1)
    bit_set(&ds_activation, 2);
}

//! set sensor mode to passive (light sensor detects ambient light)
/*! \param  sensor: &SENSOR_1,&SENSOR_2,&SENSOR_3
*/
extern inline void ds_passive(volatile unsigned *sensor)
{
  if (sensor == &SENSOR_3)
    bit_clear(&ds_activation, 0);
  else if (sensor == &SENSOR_2)
    bit_clear(&ds_activation, 1);
  else if (sensor == &SENSOR_1)
    bit_clear(&ds_activation, 2);
}

#ifdef CONF_DSENSOR_ROTATION
//! set rotation to an absolute value
/*! \param  sensor: &SENSOR_1,&SENSOR_2,&SENSOR_3

    The axis should be inert during the function call.
*/
extern void ds_rotation_set(volatile unsigned *sensor, int pos);

//! start tracking rotation sensor
/*! \param  sensor: &SENSOR_1,&SENSOR_2,&SENSOR_3
*/
extern inline void ds_rotation_on(volatile unsigned *sensor)
{
  if (sensor == &SENSOR_3)
    bit_set(&ds_rotation, 0);
  else if (sensor == &SENSOR_2)
    bit_set(&ds_rotation, 1);
  else if (sensor == &SENSOR_1)
    bit_set(&ds_rotation, 2);
}

//! stop tracking rotation sensor
/*! \param  sensor: &SENSOR_1,&SENSOR_2,&SENSOR_3
*/
extern inline void ds_rotation_off(volatile unsigned *sensor)
{
  if (sensor == &SENSOR_3)
    bit_clear(&ds_rotation, 0);
  else if (sensor == &SENSOR_2)
    bit_clear(&ds_rotation, 1);
  else if (sensor == &SENSOR_1)
    bit_clear(&ds_rotation, 2);
}
#endif // CONF_DSENSOR_ROTATION

#endif // CONF_DSENSOR

#ifdef  __cplusplus
}
#endif

#endif // __dsensor_h__
