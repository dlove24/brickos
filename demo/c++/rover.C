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

#include <c++/MotorPair.H>
#include <c++/Sound.H>
#include <conio.h>
#include <unistd.h>

//
// This is a simple example of a two motor rover which moves forward for
// 5 seconds, then goes in reverse for 5 seconds.  It does this cycle
// 5 times.
//

int 
main(int argc, 
     char **argv) 
{
  MotorPair m(Motor::A, Motor::C);

  for (int i = 0; i < 10; ++i) {
    if (i % 2) {
      m.forward();
      Sound::beep();
      cputs ("FWD");
    } else {
      m.reverse();
      cputs ("REV");
    }
    m.speed(m.max);
    sleep(5);
  }
  m.off();
  cls();
  
  return 0;
}
