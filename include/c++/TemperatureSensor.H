/*! \file   include/c++/TemperatureSensor.H
    \brief  C++ TemperatureSensor Class Interface
    \author Pat Welch (legOS@mousebrains.com)  
                 
    Defines interface to temperature sensors 
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

#ifndef _TemperatureSensor_H_
#define _TemperatureSensor_H_

#include <config.h>
#include <c++/Sensor.H>

#if defined(CONF_DSENSOR)

/**
 *  \class TemperatureSensor TemperatureSensor.H c++/TemperatureSensor.H
 * Temperature-sensor interface.
 * This is class deals with temperature sensors
 *
 * The methods include:
 * TemperatureSensor(sensor port)
 * C() returns a float which is the temperature in C.  The sensor appears 
 *     to have a resolution of ~1/8 degree.
 * F() returns a float which is the temperature in F.
 * tenths() returns an int which is the temperature in C * 10
 * degrees() returns an int which is the temperature in C
 *
 * It also includes all methods in Sensor
 *
 * N.B. the transform from sensor reading to temperature is done using
 * a simple linear regression compared to my outdoor thermometer over the
 * range of -20 to +30 C.  I can not vouch for the accuracy or precision of
 * these numbers!  So if you have a well calibrated temperature monitor, and
 * you can check my conversions, please let me know the results, and I can
 * clean up the regression parameters.
 */

class TemperatureSensor : public Sensor {
public:
 /**
   * Construct a temperature sensor.
   *
   * At time of construction identify where it is connected.
   * \param port The port to be associated with this instance
   * \see Sensor#Port
   */
  TemperatureSensor(const Sensor::Port port) : Sensor(port, false) {}

 /**
   * Destroy this instance of TemperatureSensor
   */
  ~TemperatureSensor() {}

 /**
   * Get the current temperature in Celsius
   *
   * \return float - the current temperature in C
   */
  float C() const {
    return 93.8136 - 0.122241 * static_cast<float>(get() >> 6);
  }

 /**
   * Get the current temperature in F
   *
   * \return float - the current temperature in F
   */
  float F() const {return C() * 1.8 + 32;}

 /**
   * Get the current temperature in C * 10, as an int
   *
   * \return int - the current temperature in C * 10
   */
  int tenths() const {return static_cast<int>(C() * 10.);}

 /**
   * Get the current temperature in C, as an int
   *
   * \return int - the current temperature in C
   */
  int degrees() const {return static_cast<int>(C());} 
};

#else // CONF_DSENSOR
#warning Enable CONF_DSENSOR to use TemperatureSensor.H
#endif // CONF_DSENSOR
#endif // _TemperatureSensor_H_
