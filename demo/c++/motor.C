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
#if defined(CONF_DMOTOR)

#include <c++/Motor.H>
#include <c++/Sound.H>
#include <conio.h>
#include <unistd.h>

// This is a simple example of using a single motor, which runs forward
// at the fastest speed for 1 second, then goes in reverse at 1/2 speed
// for 1 second, and then runs a minimum speed for 1 second.

int 
main(int argc, 
     char **argv) 
{
  Motor m(Motor::A);

  m.forward();
  cputs ("Fast");
  m.speed(m.max);
  sleep(1);
  cputs ("Med");
  m.speed((m.max + m.min) / 2);
  m.reverse();
#ifdef CONF_DSOUND
  Sound::beep();
#endif // CONF_DSOUND
  sleep(1);
  cputs ("Slow");
  m.speed(m.min);
  sleep(1);
  cls();
  
  return 0;
}

#else // CONF_DMOTOR
#warning motor.C requires CONF_DMOTOR
#warning motor demo will do nothing
int 
main(int argc, 
     char **argv) 
{
  return 0;
}
#endif // CONF_DMOTOR
