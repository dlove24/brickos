/*! \file   c++/Motor.H
    \brief  C++ Motor Class Interface
    \author Pat Welch (legOS@mousebrains.com)  
		 
    Defines interface to Motors plugged into the RCX
*/
// The contents of this file are subject to the Mozilla Public License
// Version 1.0 (the "License"); you may not use this file except in
// compliance with the License. You may obtain a copy of the License
// at http://www.mozilla.org/MPL/
// 
// Software distributed under the License is distributed on an "AS IS"
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
// the License for the specific language governing rights and
// limitations under the License.


// This class is designed to manipulate the motors on a MindStorm brick.  
//
// The class is Motor.  To use a motor on port A, one would do
//  Motor m(Motor::A);
//
// The methods include:
// void speed(const unsigned char s) which sets the motor speed to s
// void direction(const MotorDirection d) which sets the motor direction to d
//                                        d may be fwd, rev, off, or brake
//
// void forward() which sets the direction to fwd
// void reverse() which sets the direction to rev
// void brake() which sets the direction to brake
// void off() which sets the direction to off
//
// forward(const unsigned char s) which sets the direction to fwd, and
//                                sets the speed to s
// reverse(const unsigned char s) which sets the direction to rev, and
//                                sets the speed to s
// void brake(const int ms) which sets the direction to brake, and
//                                delays returning ms milliseconds
//

#ifndef _Motor_H_
#define _Motor_H_

#include <config.h>
#include <conio.h>
#include <dmotor.h>
 
#if defined(CONF_DMOTOR)

/**
 * \class Motor c++/Motor.H
 * This class is designed to manipulate the motors on a MindStorm brick
 * \short Motor control interface
 * \see MotorPair, Sound
 * \par Design Note
 *   I avoided using derived classes since they were typically
 *   three times larger.  Also, if one is using Motor(Motor::A), all the decision
 *   logic is optimized away in the compiler, so no extra space is used for that.
 */

class Motor {
public:
  /**
  * valid port designators
  */
  enum Port {
         A,  //!< RCX output pad A
		 B,  //!< RCX output pad B
		 C   //!< RCX output pad C
	   }; 

  /**
  *  motor speed range
  */
  enum Limits { 
           min = 0, 	//!< Minimum velocity (0)
		   max = 255	//!< Maximum velocity (255)
	   };

  /**
  *  construct a motor connected to {port}
  *  \param port designator of pad to which this motor is connected
  */
  Motor(const Port port) 
    : ms(port == A ? motor_a_speed : 
	 (port == B) ? motor_b_speed :
	 motor_c_speed), 
	 md(port == A ? motor_a_dir : 
	    (port == B) ? motor_b_dir :
	    motor_c_dir)
  { }
  /**
  *  destroy our motor instance
  * \sideeffect When the class is destroyed, the motor is turned off.
  */
  ~Motor() {off();}
  /**
  *  set the motor speed
  *  \param speed the desired setting. Must be between
  *   min (0) and max (255)
  *  \return Nothing
  */
  const void speed(const unsigned char speed) const { (*ms)(speed); }
  /**
  *  set the motor direction
  *  \param dir one of forward, reverse, off or brake
  *  \return Nothing
  */
  const void direction(const MotorDirection dir) const { (*md)(dir); }
  /**
  *  set motor direction to forward
  *  \return Nothing
  */
  const void forward() const { direction(fwd); }
  /**
  *  set motor direction to forward at speed {s}
  *  \param s  the desired setting. Must be between
  *   min (0) and max (255)
  *  \return Nothing
  */
  const void forward(const unsigned char s) const { forward(); speed(s); }
  /**
  *  set the motor direction to reverse
  *  \return Nothing
  */
  const void reverse() const { direction(rev); }
  /**
  *  set the motor direction to reverse at speed {s}
  *  \param s  the desired setting. Must be between
  *   min (0) and max (255)
  *  \return Nothing
  */
  const void reverse(const unsigned char s) const { reverse(); speed(s); }
  /**
  *  set the motor to brake
  *  \return nothing
  *  \todo describe what brake means
  */
  const void brake() const { direction(std::brake); }
  /**
  *  set the motor to brake and delay return
  *  \param duration time in mSec to delay before returning
  *  \return nothing
  *  \todo describe what brake means
  */
  const void brake(int duration) const { brake(); delay(duration); }
  /**
  *  turn the motor off
  *  this disables power and the motor coasts to a stop
  *  \return nothing
  */
  const void off() const { direction(std::off); }

private:
  void (*ms)(unsigned char speed);		//!< current velocity setting for this motor instance
  void (*md)(const MotorDirection dir);	//!< current direction setting for this motor instance
};

#else // CONF_DMOTOR
#warning Enable CONF_DMOTOR to use Motor.H
#endif // CONF_DMOTOR
#endif // _Motor_H_
