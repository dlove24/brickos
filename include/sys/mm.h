/*! \file   include/sys/mm.h
    \brief  Internal Interface: memory management
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

#ifndef __sys_mm_h__
#define __sys_mm_h__

#ifdef  __cplusplus
extern "C" {
#endif

#include <config.h>

#ifdef CONF_MM

#include <mem.h>
#include <stdlib.h>


///////////////////////////////////////////////////////////////////////
//
// Definitions
//
///////////////////////////////////////////////////////////////////////

#define MM_FREE		0x0000	      	      	//!< marker: block free
#define MM_RESERVED	0xffff	      	      	//!< marker: block reserved

// as data generally nees to be word aligned, 1 unit ^= 2 bytes
//

#define MM_HEADER_SIZE	2			//!< 2 words header: pid, size
#define MM_SPLIT_THRESH	(MM_HEADER_SIZE+8)	//!< split off if 8+ data bytes

extern size_t mm_start;				//!< end of kernel code + data

extern size_t* mm_first_free;			//!< ptr to first free block.

// Macros for mm_init()
// Always alternate FREE and RESERVED.
//

//! memory from addr on can be allocated
/*! Macro for mm_init().
    Always alternate MM_BLOCK_FREE and MM_BLOCK_RESERVED.
*/
#define MM_BLOCK_FREE(addr) 					\
	next=(size_t*)(addr);					\
	*current=((((size_t)next)-(size_t)current)-2)>>1;	\
	*(next++)=MM_FREE;					\
	current=next;


//! memory from addr on is reserved
/*! Macro for mm_init().
    Always alternate MM_BLOCK_FREE and MM_BLOCK_RESERVED.
*/
#define MM_BLOCK_RESERVED(addr) 				\
	next=(size_t*)(((size_t)addr)-4);			\
	*current=((((size_t)next)-(size_t)current)-2)>>1;	\
	*(next++)=MM_RESERVED;					\
	current=next;


///////////////////////////////////////////////////////////////////////
//
// Functions
//
///////////////////////////////////////////////////////////////////////

//! initialize memory management
extern void mm_init();

//! free all blocks allocated by the current process
extern void mm_reaper();

//! how many bytes of memory are free?
extern int mm_free_mem(void);

#endif	// CONF_MM

#ifdef  __cplusplus
}
#endif

#endif  // __sys_mm_h__
