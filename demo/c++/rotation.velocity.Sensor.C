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

#include <config.h>
#if defined(CONF_DSENSOR) && defined(CONF_DSENSOR_VELOCITY)

#include <c++/RotationSensor.H>
#include <conio.h>
#include <unistd.h>

// This program reads the rotation sensor and displays the hex value on
/// the display every 10 ms
int 
main(int argc, 
     char **argv) 
{
  RotationSensor r(RotationSensor::S2, 0);

  while (!shutdown_requested()) {
#ifdef CONF_DSENSOR_VELOCITY
    cputw(r.velocity());
#else
    cputw(0xffff);
#endif
    delay(1000);
  }
  return 0;
}
#else
#warning rotation.velocity.Sensor.C requires CONF_DSENSOR, CONF_DSENSOR_VELOCITY
#warning rotation.velocity.Sensor.C demo will do nothing.
int
main(int argc,
		 char **argv)
{
	return 0;
}
#endif // defined(CONF_DSENSOR) && defined(CONF_DSENSOR_VELOCITY)
