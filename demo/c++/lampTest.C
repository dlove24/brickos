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

// This program beeps once a second

#include <config.h>
#if defined(CONF_DMOTOR)

#include <unistd.h>		// for the sleep() func.
#include <tm.h>			// for the shutdown_requested() func.

#include <c++/Lamp.H>

int 
main(int argc, 
     char **argv) 
{
  Lamp myLite(Lamp::B);
  int power = 0;
  
  myLite.on();
  while (power < 255 && !shutdown_requested()) {
	myLite.brightness(power);
	sleep(1);
	power += (256/8);
  }
  myLite.off();
  return 0;
}
#else // CONF_DMOTOR
#warning lampTest.C requires CONF_DMOTOR
#warning lamp demo will do nothing
int 
main(int argc, 
     char **argv) 
{
  return 0;
}
#endif // CONF_DMOTOR
