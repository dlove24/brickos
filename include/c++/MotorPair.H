/*! \file   include/c++/MotorPair.H
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

#if defined(CONF_DMOTOR)

#include <conio.h>	// for the delay() function
#include <c++/Motor.H>

/// \class MotorPair MotorPair.H c++/MotorPair.H
/// Pair-of-motors control interface.
///
/// Often our robots use two motors, one on either side, which should be controlled
/// together as one.  This class makes this simple by allowing the coder to treat 
/// the pair as a single motor.
/// 
/// The two motors can be connected to any of the output ports.  When creating an 
/// instance of this class, simply specify which two output ports are involved.
///
/// The normal Motor methods speed(), direction(), forward(), reverse(), brake(), 
/// and off() still apply.
/// 
/// This class adds two new types of control: 
/// \li turning about the center (between the
/// two wheels) [left(), right()]
///
/// \li turning while using one of the wheels as the 
/// center of rotation [pivotLeft() and pivotRight()].
///
/// \note both motors will be turned off when this class is
/// destroyed.
///
/// \see The other control classes: Motor, Sound, Lamp
///
class MotorPair {
public:

  ///  define a pair of motors specifying the connection of each to the
  ///  RCX
  MotorPair(const Motor::Port lport, const Motor::Port rport)
    : mLeft(lport), mRight(rport) {}

  ///  destroy this MotorPair instance
  ///  \note both motors will be turned off when this class is destroyed
  ~MotorPair() {}

  ///  set the speed of our two motors
  void speed(const int s) const { mLeft.speed(s); mRight.speed(s); }

  ///  set the direction of our two motors
  ///  \param dir one of the MotorDirection values
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

  ///  move forward
  void forward() const { direction(std::fwd); }

  ///  move reverse
  void reverse() const { direction(std::rev); }

  ///  stop without coasting
  void brake() const { 
    mLeft.direction(std::brake); 
    mRight.direction(std::brake); 
  }

  ///  stop but allow coasting
  void off() const { 
    mLeft.direction(std::off); 
    mRight.direction(std::off); 
  }

  ///  turn left about the center of the robot
  ///  \note both motors in the pair are turning
  void left() const { 
    mLeft.direction(std::fwd); 
    mRight.direction(std::fwd); 
  }

  ///  turn left about the left wheel of the robot
  ///  \note the left motor is brake'd while the right motor is turning
  void pivotLeft() const { 
    mLeft.brake(); 
    mRight.direction(std::rev); 
  }

  ///  turn right about the center of the robot
  ///  \note both motors in the pair are turning
  void right() const { 
    mLeft.direction(std::rev); 
    mRight.direction(std::rev); 
  }

  ///  turn right about the right wheel of the robot
  ///  \note the right motor is brake'd while the left motor is turning
  void pivotRight() const { 
    mLeft.direction(std::fwd); 
    mRight.brake(); 
  }


  ///  move forward at speed {s}
  ///  \param s the desired speed (power level)
  void forward(const int s) const { forward(); speed(s); }

  ///  move reverse (go backwards) at speed {s}
  ///  \param s the desired speed (power level)
  void reverse(const int s) const { reverse(); speed(s); }

  ///  turn left at speed {s}
  ///  \param s the desired speed (power level)
  ///  \note spins about the center of the motor pair
  void left(const int s) const { left(); speed(s); }

  ///  turn left at speed {s} but pivot around left wheel
  ///  \param s the desired speed (power level)
  ///  \note spins about the left wheel
  void pivotLeft(const int s) const { pivotLeft(); speed(s); }

  ///  turn right at speed {s}
  ///  \param s the desired speed (power level)
  ///  \note spins about the center of the motor pair
  void right(const int s) const { right(); speed(s); }

  ///  turn right at speed {s} but pivot around right wheel
  ///  \param s the desired speed (power level)
  ///  \note spins about the right wheel
  void pivotRight(const int s) const { pivotRight(); speed(s); }

  ///  apply the brakes to both motors then delay for {ms} mSec
  ///  \param ms the time in mSec to wait before returning to caller
  void brake(const int ms) const { brake(); delay(ms); }

  ///  the minimum and maximum speeds a motor can go, its limits.
  enum Limits {
         min = Motor::min,  //!< minimum motor speed setting
		 max = Motor::max   //!< maximum motor speed setting
	   };

private:
  const Motor mLeft;	//!< the left Motor instance
  const Motor mRight;	//!< the right Motor instance
};

#else // CONF_DMOTOR
#warning Enable CONF_DMOTOR to use MotorPair.H
#endif // CONF_DMOTOR
#endif // _MotorPair_H_
