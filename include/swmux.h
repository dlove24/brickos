/*this code applies to the touch sensor multiplexor (swmux) that can be ordered
  from www.techno-stuff.com, tweaking the ranges in swmux.c should make it
  work with other similar multiplexors
  Author: Mark Falco, send questions or comments to falcom@onebox.com
*/
#ifndef SWMUX_H
#define SWMUX_H

#ifdef  __cplusplus
extern "C" {
#endif

#include <config.h>
#ifdef CONF_DSENSOR
#ifdef CONF_DSENSOR_SWMUX



/*bit masks for for checking the parsed value*/
#define SWMUX_A_MASK 1
#define SWMUX_B_MASK 2
#define SWMUX_C_MASK 4
#define SWMUX_D_MASK 8
#define SWMUX_ERROR_MASK 16 //value not recognized


/*Takes in raw sensor values and converts into a bitfield
  indicating the state of the attached touch sensors.
  If there is an error interepting the raw sensor value
  the error bit will be set, in which case the other bits
  should be ignored.  The bitfield is EDCBA, and can be examined
  by "anding" the field with the above masks.  The E in EDCBA is
  error in case you're wondering.
*/
unsigned char swmux_bits(unsigned int raw);

  //these macros will make do the anding for  you
#define SWMUX_A(bitfield) (bitfield & SWMUX_A_MASK)
#define SWMUX_B(bitfield) (bitfield & SWMUX_B_MASK)
#define SWMUX_C(bitfield) (bitfield & SWMUX_C_MASK)
#define SWMUX_D(bitfield) (bitfield & SWMUX_D_MASK)
#define SWMUX_ERROR(bitfield) (bitfield & SWMUX_ERROR_MASK)

#ifdef  __cplusplus
}//extern C
#endif

#endif //CONF_DSENSOR_SWMUX
#endif //CONF_DSENSOR

#endif //SWMUX_H
