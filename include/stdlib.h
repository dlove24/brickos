/*! \file   include/stdlib.h
    \brief  Interface: reduced standard C library
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

#ifndef __stdlib_h__
#define __stdlib_h__

#ifdef  __cplusplus
extern "C" {
#endif

#include <mem.h>

///////////////////////////////////////////////////////////////////////
//
// Functions
//
///////////////////////////////////////////////////////////////////////

//
// Standard memory management.
// See the Linux man pages for details.
//

extern void *calloc(size_t nmemb, size_t size);

extern void *malloc(size_t size);

extern void free(void *ptr);

extern long int random(void);
extern void srandom(unsigned int seed);

#ifdef  __cplusplus
}
#endif

#endif // __stdlib_h__
