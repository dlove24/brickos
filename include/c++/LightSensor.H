/*! \file   include/c++/LightSensor.H
    \brief  C++ LightSensor Class Interface
    \author Pat Welch (legOS@mousebrains.com)  
		 
    Defines interface to LightSensors plugged into the RCX
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

#ifndef _LightSensor_H_
#define _LightSensor_H_

#include <config.h>
#include <c++/Sensor.H>

#if defined(CONF_DSENSOR)
/**
 * \class LightSensor LightSensor.H c++/LightSensor.H
 * Light-sensor interface (active/passive mode).
 * The LightSensor Class - using a %LightSensor in active or passive mode
 *
 * At construction time you specify the RCX connector pad to which 
 * the %LightSensor is attached.
 *
 * This class provides two methods to read the value of the 
 * sensor [\ref #get, \ref #sample]
 *
 * \note access methods from the base class \ref Sensor 
 * if you desire access to the raw sensor values or to the power control 
 * methods.
 *
 * \see The other sensor classes: Sensor, RotationSensor, and TouchSensor
 */
class LightSensor : public Sensor {
public:
  /**
   * Instance an active light sensor at {port}
   * \param port The connector pad (1, 2 or 3) to which the light sensor is 
   * attached
   */
  LightSensor(const Sensor::Port port) : Sensor(port, true) {}
  /**
   * Get the current lightSensor value.
   *
   * NOTE: ultimately, this will be scaled to fall between 0 and 100.
   * \return unsigned int - the current light sensor reading
   * \see LightSensor#sample
   */
  unsigned int get() const {return LIGHT(sensor);}
  /**
   * Get the average of {size} samples, waiting {wait} mSec between each sample
   * \param size - Number of samples to average (default = 10 samples)
   * \param wait - time (in mS) to wait between samples (default = 2mS)
   * \return unsigned int - The average of the sampled values
   * \see LightSensor#get
   */
  unsigned int sample(unsigned int size = 10, int wait = 2) const {
    if (size == 0) size = 1;
    unsigned int sum(get());
    for (unsigned int i = 1; i < size; ++i) {
      sum += get();
      delay(wait);
    }
    return sum / size;
  }
};

#else // CONF_DSENSOR
#warning Enable CONF_DSENSOR to use LightSensor.H
#endif // CONF_DSENSOR
#endif // _LightSensor_H_
