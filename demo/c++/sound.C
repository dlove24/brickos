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
#if defined(CONF_DSOUND)

#include <c++/Sound.H>
#include <conio.h>
#include <unistd.h>
#include <tm.h>

int 
main(int argc, 
     char **argv) 
{
  while (!shutdown_requested()) {
    Sound::beep();
    cputs ("Beep");
    sleep(1);
    cls();
  }
  cls();
  
  return 0;
}
#else // CONF_DSOUND
#warning sound.C requires CONF_DSOUND
#warning sound demo will do nothing
int 
main(int argc, 
     char **argv) 
{
  return 0;
}
#endif // CONF_DSOUND
