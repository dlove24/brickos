/*! \file   include/c++/Lamp.H
    \brief C++ Lamp Class Interface
    \author Stephen M Moraco (stephmo@users.sourceforge.net)

    Defines interface to Lamp control via the output ports of the RCX
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
// Contributor: Stephen M Moraco (stephmo@users.sourceforge.net)

#ifndef _Lamp_H_
#define _Lamp_H_

#include <config.h>

#if defined(CONF_DMOTOR)

#include <dmotor.h>

/**
 * \class Lamp Lamp.H c++/Lamp.H
 * Lamp control interface.
 * Define a lamp connection to the RCX so we can turn the lamp on(), off()
 * or set its brightness().
 * \note Lamps are controlled via motor ports so this is based on the Motor.H code
 * \see The other control classes: Motor, MotorPair, Sound
 */
class Lamp {
public:
  /**
   *  The RCX output pad to which the Lamp is connected
   */
  enum Port { 
         A, //!< RCX output pad A
         B, //!< RCX output pad B
		 C  //!< RCX output pad C
	   }; 

  /**
   *  construct a lamp connected to {port}
   *  \param port designator of pad to which this lamp is connected
   */
  Lamp(enum Port port) 
     : ms(port == A ? motor_a_speed : 
          (port == B) ? motor_b_speed :
          motor_c_speed), 
       md(port == A ? motor_a_dir : 
          (port == B) ? motor_b_dir :
          motor_c_dir)
   { }
  /**
   *  destroy our lamp instance
   *  \note the lamp is turned off when the instance is destroyed
   */
  ~Lamp() { off(); }
  /**
   * Turn the Lamp on
   * \return Nothing
   */
  const void on() const { direction(fwd); }
  /**
   * Turn the Lamp off
   * \return Nothing
   */
  const void off() const { direction(std::off); }
  /**
   * Set the power to the lamp
   * \param level the desired power setting specifying the intensity of the light. 
     [must be between min (0) and max (255)]
   * \return Nothing
   */
  const void brightness(const unsigned char level) const { speed(level); }
private:
  const void speed(const unsigned char speed) const { (*ms)(speed); }
  const void direction(const MotorDirection dir) const { (*md)(dir); }

  void (*ms)(unsigned char speed);              
  void (*md)(const MotorDirection dir); 
};

#else // CONF_DMOTOR
#warning Enable CONF_DMOTOR to use Lamp.H
#endif // CONF_DMOTOR

#endif // _Lamp_H_
