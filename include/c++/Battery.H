/*! \file   include/c++/Battery.H 
    \brief  C++ Battery Class Interface
    \author Pat Welch (legOS@mousebrains.com)  
		 
    Defines interface to RCX-internal battery sensor
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

#ifndef _Battery_H_
#define _Battery_H_

#include <config.h>
#include <sys/battery.h>

#if defined(CONF_DSENSOR)
/**
 * \class Battery Battery.H c++/Battery.H
 * Battery-sensor interface (RCX Internal).
 * This class deals with the battery sensor. 
 *
 *  It provides only the single get() method.
 *
 * \see The other sensor classes: Sensor, LightSensor, RotationSensor, TouchSensor
 */
class Battery {
public:
  /**
   * get the current battery strength value
   * \return int current battery reading in milliVolts (mV)
   * \note this is the c++ implementation of get_battery_mv()
   */
  static int get() {return get_battery_mv();}
};

#else // CONF_DSENSOR
#warning Enable CONF_DSENSOR to use Battery.H
#endif // CONF_DSENSOR
#endif // _Battery_H_
