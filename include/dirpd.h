/*this code applies to the Dual IR Proximity Detector  that can be ordered
  from www.techno-stuff.com.  You may need to modify the ranges
  Author: Mark Falco, send questions or comments to falcom@onebox.com
*/

#ifndef DIRPD_H
#define DIRPD_H

//0xdec0 - sampled value for object to left
#define DIRPD_LEFT_S 0xd000 
#define DIRPD_LEFT_E 0xe000

//0xb700 - sampled value for object to right
#define DIRPD_RIGHT_S 0xb000
#define DIRPD_RIGHT_E 0xbfff

//0xffc0 - sampled value for object to center
#define DIRPD_CENTER_S 0xf000
#define DIRPD_CENTER_E 0xffff

//0x8d40 - sampled value for no object
#define DIRPD_NONE_S 0x8000
#define DIRPD_NONE_E 0x8fff


//pass in a value obtained from an active sensor

#define DIRPD_LEFT(araw) (araw >= DIRPD_LEFT_S && araw <= DIRPD_LEFT_E)
#define DIRPD_RIGHT(araw) (araw >= DIRPD_RIGHT_S && araw <= DIRPD_RIGHT_E)
#define DIRPD_CENTER(araw) (araw >= DIRPD_CENTER_S && araw <= DIRPD_CENTER_E)
#define DIRPD_NONE(araw) (araw >= DIRPD_NONE_S && araw <= DIRPD_NONE_E)


#endif //DIRPD_H
