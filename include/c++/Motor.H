/*! \file   include/c++/Motor.H
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
//
// This software was developed as part of the legOS project.
//
// Contributor: Pat Welch (legOS@mousebrains.com)
//
//
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
// void Brake(const int ms) which sets the direction to brake, and
//                                delays returning ms milliseconds
//
// When the class is destroyed, the motor is turned off.
//
// On a design note, I avoided using derived classes since they were typically
// three times larger.  Also, if one using Motor(Motor::A), all the decision
// logic is optimized away in the compiler, so no extra space is used for that.
//

#ifndef _Motor_H_
#define _Motor_H_

#include <conio.h>
#include <dmotor.h>
 
#if defined(CONF_DMOTOR)


/**
 * @class Motor Motor.H <c++/Motor.H>
 * This class is designed to manipulate the motors on a MindStorm brick
 * @short Motor Control interface
 * @see MotorPair, Sound
 */

class Motor {
public:
  enum Port { A, B, C }; 
  enum Limits { min = 0, max = 255 };

  Motor(const Port port) 
    : ms(port == A ? motor_a_speed : 
	 (port == B) ? motor_b_speed :
	 motor_c_speed), 
	 md(port == A ? motor_a_dir : 
	    (port == B) ? motor_b_dir :
	    motor_c_dir)
  { }
  ~Motor() {off();}
  const void speed(const unsigned char speed) const { (*ms)(speed); }
  const void direction(const MotorDirection dir) const { (*md)(dir); }
  const void forward() const { direction(fwd); }
  const void forward(const unsigned char s) const { forward(); speed(s); }
  const void reverse() const { direction(rev); }
  const void reverse(const unsigned char s) const { reverse(); speed(s); }
  const void brake() const { direction(std::brake); }
  const void brake(int duration) const { brake(); delay(duration); }
  const void off() const { direction(std::off); }

private:
  void (*ms)(unsigned char speed);
  void (*md)(const MotorDirection dir);
};

#else // CONF_DMOTOR
#warning Enable CONF_DMOTOR to use Motor.H
#endif // CONF_DMOTOR
#endif // _Motor_H_
