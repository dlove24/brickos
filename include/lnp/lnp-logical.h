/*! \file   include/lnp/lnp-logical.h
    \brief  LNP Interface: link networking protocol logical layer
    \author Markus L. Noga <markus@noga.de>
*/

/*
 *  The contents of this file are subject to the Mozilla Public License
 *  Version 1.0 (the "License"); you may not use this file except in
 *  compliance with the License. You may obtain a copy of the License
 *  at http://www.mozilla.org/MPL/
 *
 *  Software distributed under the License is distributed on an "AS IS"
 *  basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 *  the License for the specific language governing rights and
 *  limitations under the License.
 *
 *  The Original Code is legOS code, released October 17, 1999.
 *
 *  The Initial Developer of the Original Code is Markus L. Noga.
 *  Portions created by Markus L. Noga are Copyright (C) 1999
 *  Markus L. Noga. All Rights Reserved.
 *
 *  Contributor(s): Markus L. Noga <markus@noga.de>
 */

#ifndef __lnp_logical_h__
#define __lnp_logical_h__

#ifdef  __cplusplus
extern "C" {
#endif

#include <config.h>

#ifdef CONF_LNP

#ifdef CONF_HOST
#include <stddef.h>
#else
#include <mem.h>
#include <sys/h8.h>
#endif

///////////////////////////////////////////////////////////////////////
//
// Functions
//
///////////////////////////////////////////////////////////////////////

#ifndef CONF_HOST
//! set IR transmitter range
/*! \param far 0: short range, 1: long range
    toggles port 4 bit 0
*/
extern inline void lnp_logical_range(int far) {
  if(far)
    *((char*)&PORT4) &=~1;
  else
    *((char*)&PORT4) |=1;
}

//! return IR transmitter range
extern inline int lnp_logical_range_is_far() {
  return !(*((char*)&PORT4)&1);
}
#endif

//
// write to IR port, blocking.
// return code: 0 if OK, else collision
//
extern int lnp_logical_write(const void *buf,size_t len);

//
// flush input buffer.
//
extern void lnp_logical_fflush(void);

#endif  // CONF_LNP

#ifdef  __cplusplus
}
#endif

#endif  // __lnp_logical_h__

