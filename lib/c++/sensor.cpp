/*! \file   c++/sensor.cpp
    \brief  Direct sensor access in C++
    \author Markus L. Noga <markus@noga.de>
*/
    
/*
 *  The contents of this file are subject to the Mozilla Public License
 *  Version 1.0 (the "License"); you may not use this file except in
 *  compliance with the License. You may obtain a copy of the License at
 *  http://www.mozilla.org/MPL/
 *
 *  Software distributed under the License is distributed on an "AS IS"
 *  basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 *  License for the specific language governing rights and limitations
 *  under the License.
 *
 *  The Original Code is legOS code, released October 17, 1999.
 *
 *  The Initial Developer of the Original Code is Markus L. Noga.
 *  Portions created by Markus L. Noga are Copyright (C) 1999
 *  Markus L. Noga. All Rights Reserved.
 *
 *  Contributor(s): Markus L. Noga <markus@noga.de>
 */

#include "c++/sensor.h"


unsigned* const Sensor::s1=&SENSOR_1;         //!< RCX sensor 1 address
unsigned* const Sensor::s2=&SENSOR_2;         //!< RCX sensor 2 address
unsigned* const Sensor::s3=&SENSOR_3;         //!< RCX sensor 3 address
unsigned* const Sensor::battery=&BATTERY;     //!< RCX battery sensor address


RotationSensor::RotationSensor(unsigned *addr,int initial) : Sensor(addr,1) {
  ds_rotation_set(ptr,initial);
  ds_rotation_on(ptr);
  if(ptr==s1)
    posPtr=(int * volatile) ds_rotations+2;
  else if(ptr==s2)
    posPtr=(int * volatile) ds_rotations+1;
  else if(ptr==s3)
    posPtr=(int * volatile) ds_rotations;
}
