#ifndef _TouchSensor_H_
#define _TouchSensor_H_

/*! \file   include/c++/TouchSensor.H
    \brief  C++ TouchSensor Class Interface
    \author Pat Welch (legOS@mousebrains.com)  
		 
    Defines the interface to a TouchSensor plugged into RCX
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

#include <c++/Sensor.H>

#if defined(CONF_DSENSOR)

/**
 * @class TouchSensor TouchSensor.H <c++/TouchSensor.H>
 * The Touch Sensor Class - using a touch sensor in passive mode 
 *
 * At construction time you specify the RCX connector pad to which the 
 * touchSensor is attached. 
 *
 * @note TouchSensor is implemented as a simple redefine of the Sensor 
 * class (saved ~48 bytes per instance).  See "list of all members", above, or
 * refer to the Sensor class for all methods
 *
 * @short Touch Sensor interface (passive mode)
 * @see Sensor, LightSensor, RotationSensor
 */

class TouchSensor : public Sensor {
public:
  TouchSensor(const Sensor::Port port) : Sensor(port, false) {}
  bool pressed() const {return TOUCH(sensor);}
};

#else // CONF_DSENSOR
#warning Enable CONF_DSENSOR to use TouchSensor.H
#endif // CONF_DSENSOR
#endif // _TouchSensor_H_
