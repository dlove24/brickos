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
#if defined(CONF_DMOTOR) && defined(CONF_DSENSOR)

#include <c++/MotorPair.H>
#include <c++/LightSensor.H>
#include <conio.h>
#include <unistd.h>
#include <tm.h>

//
// This is a naive example of a rover which follows a dark line
//

int 
main(int argc, 
     char **argv) 
{
  MotorPair m(Motor::A, Motor::C);
  LightSensor l(LightSensor::S2);

  const int speed = (m.max + m.min) / 3;
  m.off();

  unsigned int lightLevel(l.sample());
  unsigned int threshold(lightLevel + 5);

  cputw (lightLevel);
  sleep(2);

  while (!shutdown_requested()) {
    m.forward(speed);
    while (1) {
      const unsigned int value(l.get());
      cputw(value);
      if (value > threshold) 
	break;
    }
    m.brake(100);
    m.left(m.max);
    while (1) {
      const unsigned int value(l.get());
      cputw(value);
      if (value < threshold) 
	break;
    }
    m.brake(100);
  }
  return 0;
}
#else // CONF_DMOTOR && CONF_DSENSOR
#warning raceTrack.C requires CONF_DMOTOR && CONF_DSENSOR
#warning raceTrack demo will do nothing
int 
main(int argc, 
     char **argv) 
{
  return 0;
}
#endif // CONF_DMOTOR && CONF_DSENSOR
