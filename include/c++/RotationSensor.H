/*! \file   include/c++/RotationSensor.H
    \brief  C++ RotationSensor Class Interface
    \author Pat Welch (legOS@mousebrains.com)  
		 
    Defines interface to RotationSensors plugged into the RCX
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

#ifndef _RotationSensor_H_
#define _RotationSensor_H_

#include <config.h>
#include <c++/Sensor.H>
#include <dsensor.h>

#if defined(CONF_DSENSOR) && defined(CONF_DSENSOR_ROTATION)

/**
 * \class RotationSensor RotationSensor.H c++/RotationSensor.H
 * Rotation-sensor interface (active mode).
 * Provide an interface to rotation sensor hardware attached to the RCX
 *
 * At construction time you specify the RCX connector pad to
 * which the rotation sensor is attached and its initial position 
 * (default = 0).
 *
 * Additionally, RotationSensor provides methods to:
 * \li enable and disable rotation sensing [\ref #on, \ref #off]
 * \li set or read the current position
 *
 * \see Sensor, LightSensor, TouchSensor
 */
class RotationSensor : public Sensor {
public:
  /**
   * Create an instance of an active rotation sensor which is attached at {port}
   * \param port the pad to which the rotation sensor is attached
   * \param position initial starting position (default = 0)
   */
  RotationSensor(const Port port, int position = 0)
    : Sensor((Sensor::Port) port, true), 
    rsensor((port == S1) ? ROTATION_1 :
	    (port == S2) ? ROTATION_2 :
	    ROTATION_3)
#ifdef CONF_DSENSOR_VELOCITY
    ,
    rvelocity((port == S1) ? VELOCITY_1 :
	      (port == S2) ? VELOCITY_2 :
	      VELOCITY_3)
#endif // CONF_DSENSOR_VELOCITY
  {
    on();          //! Turn on tracking of the sensor
    pos(position); //! Set the current position
  }
  /**
   * Destroy this instance
   */
  ~RotationSensor()
  {
    off(); //! Turn off tracking of this sensor
  }

  /**
   *  turn on rotation tracking
   */
  void on() const {ds_rotation_on(&sensor);}

  /**
   *  turn off rotation tracking
   */
  void off() const {ds_rotation_off(&sensor);}

  /**
   *  set the current absolute postion
   * \param position the absolute value to use (typically zero)
   */
  void pos(int position) const {ds_rotation_set(&sensor, position);}
  
  /**
   *  get the current absolute postion
   * \return int the current relative position 
   */
  int pos() const {return rsensor;}
#ifdef CONF_DSENSOR_VELOCITY
  /**
   *  get the current rotational velocity
   * \return int the current velocity
   */
  int velocity() const {return rvelocity;}
#endif // CONF_DSENSOR_VELOCITY

private:
  /**
   *  the raw sensor value
   */
  volatile int& rsensor;
#ifdef CONF_DSENSOR_VELOCITY
  /**
   *  the raw velocity value
   */
  volatile int& rvelocity;
#endif // CONF_DSENSOR_VELOCITY
};

#else
#warning Enable CONF_DSENSOR && CONF_DSENSOR_ROTATION to use RotationSensor.H
#endif // CONF_DSENSOR, CONF_DSENSOR_ROTATION
#endif // _RotationSensor_H_
