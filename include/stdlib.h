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

//! allocate and return pointer to initialized memory
/*! calloc() allocates memory for an array of {nmemb} elements of {size} bytes
 *  each and returns a pointer to the allocated memory.  The memory is  set
 *  to zero.
 *  \param nmemb the number of members to allocate
 *  \param size the size (in bytes) of each member to be allocated
 *  \return a pointer to the allocated memory (or NULL if failed)
 *
 *  NOTE: the memory is set to zero.
 *
 */
extern void *calloc(size_t nmemb, size_t size);

//! allocate and return pointer to uninitialized memory
/*! malloc()  allocates  {size}  bytes of memory and returns a pointer to it.
 *  \param size the number of bytes of memory to be allocated
 *  \return a pointer to the allocated memory (or NULL if failed)
 *
 *  NOTE: The memory is not set to zero.
 *  \bug multiplication overflow is not detected
 */
extern void *malloc(size_t size);

//! return the allocated memory to memory management.
/*! free() frees the memory space pointed to by {ptr}, which must  have  been
 *  returned by a previous call to malloc(), or calloc().  Other-
 *  wise, or  if  free(ptr)  has  already  been  called  before,  undefined
 *  behaviour occurs.  If ptr is NULL, no operation is performed.
 *  \param ptr a pointer to previously allocated memory
 *  \return Nothing
 */
extern void free(void *ptr);

extern long int random(void);
extern void srandom(unsigned int seed);

#ifdef  __cplusplus
}
#endif

#endif // __stdlib_h__
