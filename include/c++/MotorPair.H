/*! \file   c++/MotorPair.H
    \brief  C++ MotorPair Class Interface
    \author Pat Welch (legOS@mousebrains.com)  
		 
    Defines interface to a pair of Motors to be treated as one unit
*/
//
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

#ifndef _MotorPair_H_
#define _MotorPair_H_

#include <config.h>
#include <c++/Motor.H>

#if defined(CONF_DMOTOR)

// This class is for manipulating a simple pair of motors, that are the drive
// motors for a rover.  i.e. you have a left and right motor which drive a
// rover.  The left motor is spinning forwards to move forwards, while the
// right motor is spinning backwards.
//
// See Motor for methods speed, direction, forward, reverse, brake, and off.
// Additional methods include:
//
//  left()     Turns left by running both motors in reverse, so it should spin
//             about the center of the two motors.
//
//  pivotLeft() Turns left by locking up the left motor, and running the right
//              motor to pivot about the left wheel.
//
//  right()    Turns right by running both motors in forward, so it should spin
//             about the center of the two motors.
//
//  pivotRight() Turns right by locking up the right motor, and running the 
//               left motor to pivot about the right wheel.
//
// There are corresponding speed setting methods for the above methods,
// left(int speed), which runs the motors at the specified speed.
//

/**
 * \class MotorPair c++/MotorPair.H
 * \short Pair-of-motors control interface
 * \see Motor, Sound, Lamp
 */
 
class MotorPair {
public:
  MotorPair(const Motor::Port lport, const Motor::Port rport)
    : mLeft(lport), mRight(rport) {}

  void speed(const int s) const { mLeft.speed(s); mRight.speed(s); }
  void direction(const MotorDirection dir) const { 
    if (dir == std::fwd) {
      mLeft.direction(std::fwd); 
      mRight.direction(std::rev); 
    } else if (dir == std::rev) {
      mLeft.direction(std::rev); 
      mRight.direction(std::fwd); 
    } else {
      mLeft.direction(dir); 
      mRight.direction(dir); 
    }
  }
  void forward() const { direction(std::fwd); }
  void reverse() const { direction(std::rev); }
  void brake() const { 
    mLeft.direction(std::brake); 
    mRight.direction(std::brake); 
  }
  void off() const { 
    mLeft.direction(std::off); 
    mRight.direction(std::off); 
  }
  void left() const { 
    mLeft.direction(std::fwd); 
    mRight.direction(std::fwd); 
  }
  void pivotLeft() const { 
    mLeft.brake(); 
    mRight.direction(std::rev); 
  }
  void right() const { 
    mLeft.direction(std::rev); 
    mRight.direction(std::rev); 
  }
  void pivotRight() const { 
    mLeft.direction(std::fwd); 
    mRight.brake(); 
  }

  void forward(const int s) const { forward(); speed(s); }
  void reverse(const int s) const { reverse(); speed(s); }
  void left(const int s) const { left(); speed(s); }
  void pivotLeft(const int s) const { pivotLeft(); speed(s); }
  void right(const int s) const { right(); speed(s); }
  void pivotRight(const int s) const { pivotRight(); speed(s); }

  void brake(const int d) const { brake(); delay(d); }

  enum Limits {min = Motor::min, max = Motor::max};

private:
  const Motor mLeft;
  const Motor mRight;
};

#else // CONF_DMOTOR
#warning Enable CONF_DMOTOR to use MotorPair.H
#endif // CONF_DMOTOR
#endif // _MotorPair_H_
