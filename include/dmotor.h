/*! \file   include/dmotor.h
    \brief  Interface: direct motor control
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

#ifndef	__dmotor_h__
#define __dmotor_h__

#ifdef  __cplusplus
extern "C" {
#endif

#include <config.h>

#ifdef CONF_DMOTOR

///////////////////////////////////////////////////////////////////////
//
// Definitions
//
///////////////////////////////////////////////////////////////////////

//! the motor directions
typedef enum {
  off = 0,			//!< freewheel
  fwd = 1,			//!< forward
  rev = 2,			//!< reverse
  brake = 3			//!< hold current position

} MotorDirection;

#ifndef DOXYGEN_SHOULD_SKIP_INTERNALS
//! the motor status type.
typedef struct {
  union {
    unsigned assembler;		//!< assures word alignment for assembler

    struct {
      unsigned char delta;	//!< the speed setting

      volatile unsigned char sum;	//!< running sum

    } c;
  } access;			//!< provides access from C and assembler

  unsigned char dir;		//!< output pattern when sum overflows

} MotorState;
#endif  // DOXYGEN_SHOULD_SKIP_INTERNALS

#define  MIN_SPEED	0     	//!< minimum motor speed
#define  MAX_SPEED	255   	//!< maximum motor speed

///////////////////////////////////////////////////////////////////////
//
// Variables
//
///////////////////////////////////////////////////////////////////////

//! motor drive patterns
/*! to be indexed with MotorDirections
   \sa MotorDirections
 */
extern const unsigned char	dm_a_pattern[4],
				dm_b_pattern[4],
				dm_c_pattern[4];

extern MotorState	dm_a,	//!< motor A state
			dm_b,	//!< motor B state
			dm_c;	//!< motor C state

///////////////////////////////////////////////////////////////////////
//
// Functions
//
///////////////////////////////////////////////////////////////////////

#ifdef CONF_VIS

/*
** motor_*_dir() functions will display direction arrows, so
** define them in kernel/dmotor.c
*/
extern void motor_a_dir(MotorDirection dir);	//!< set motor A direction to dir
extern void motor_b_dir(MotorDirection dir);	//!< set motor B direction to dir
extern void motor_c_dir(MotorDirection dir);	//!< set motor C direction to dir

#else

/*
** No display, so make these functions inline
*/
//! set motor A direction
/*! \param dir the direction
 */
extern inline void motor_a_dir(MotorDirection dir)
{
  dm_a.dir = dm_a_pattern[dir];
}

//! set motor B direction
/*! \param dir the direction
 */
extern inline void motor_b_dir(MotorDirection dir)
{
  dm_b.dir = dm_b_pattern[dir];
}

//! set motor C direction
/*! \param dir the direction
 */
extern inline void motor_c_dir(MotorDirection dir)
{
  dm_c.dir = dm_c_pattern[dir];
}

#endif // ifdef CONF_VIS


//! set motor A speed
/*! \param speed the speed
 */
extern inline void motor_a_speed(unsigned char speed)
{
  dm_a.access.c.delta = speed;
}

//! set motor B speed
/*! \param speed the speed
 */
extern inline void motor_b_speed(unsigned char speed)
{
  dm_b.access.c.delta = speed;
}

//! set motor C speed
/*! \param speed the speed
 */
extern inline void motor_c_speed(unsigned char speed)
{
  dm_c.access.c.delta = speed;
}

#endif // CONF_DMOTOR

#ifdef  __cplusplus
}
#endif

#endif // __dmotor_h__
