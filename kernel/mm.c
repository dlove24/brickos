/*! \file   mm.c
    \brief  dynamic memory management
    \author Markus L. Noga <markus@noga.de>
*/
    
/*
 *  The contents of this file are subject to the Mozilla Public License
 *  Version 1.0 (the "License"); you may not use this file except in
 *  compliance with the License. You may obtain a copy of the License at
 *  http://www.mozilla.org/MPL/
 *
 *  Software distributed under the License is distributed on an "AS IS"
 *  basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 *  License for the specific language governing rights and limitations
 *  under the License.
 *
 *  The Original Code is legOS code, released October 17, 1999.
 *
 *  The Initial Developer of the Original Code is Markus L. Noga.
 *  Portions created by Markus L. Noga are Copyright (C) 1999
 *  Markus L. Noga. All Rights Reserved.
 *
 *  Contributor(s): Markus L. Noga <markus@noga.de>
 */

#include <sys/mm.h>

#ifdef CONF_MM

#include <stdlib.h>
#include <sys/tm.h>
#include <semaphore.h>
#include <string.h>

///////////////////////////////////////////////////////////////////////////////
//
// Variables
//
///////////////////////////////////////////////////////////////////////////////
      
size_t *mm_first_free;				//!< first free block

#ifndef CONF_TM
typedef size_t pid_t;                           //! dummy process ID type

//! current process ID
/*! we need a non-null, non-0xffff current pid even if there is no
    task management.
*/
const pid_t cpid=0x0001;
#else
sem_t	mm_semaphore;				//!< assures tasksafe operation
#endif

///////////////////////////////////////////////////////////////////////////////
//
// Functions
//
///////////////////////////////////////////////////////////////////////////////

//
// memory block structure:
// 0 1       : pid of owner (0=empty)
// 2 3       : size of data block >> 1
// 4 ... 4+2n: data
//

//! check for free blocks after this one and join them if possible
/* \param ptr pointer to size field of current block
   \return size of block
*/
size_t mm_try_join(size_t *ptr) {
	size_t *next=ptr+*ptr+1;
	size_t increase=0;
	
	while(*next==MM_FREE) {
		increase+=*(next+1) + MM_HEADER_SIZE;
		next    +=*(next+1) + MM_HEADER_SIZE;
	}
	return (*ptr)+=increase;
}

//! update first free block pointer
/*! \param start pointer to owner field of a memory block to start with.
*/
void mm_update_first_free(size_t *start) {
	size_t *ptr=start;
	
	while((*ptr!=MM_FREE) && (ptr>=&mm_start))
		ptr+=*(ptr+1)+MM_HEADER_SIZE;

	mm_first_free=ptr;
}


//! initialize memory management
/*!
*/
void mm_init() {
	size_t *current,*next;
	
	current=&mm_start;

	// memory layout
	//
	MM_BLOCK_FREE    (&mm_start); 	// ram
	
	      	      	      	      	// something at 0xc000 ?
	
	MM_BLOCK_RESERVED(0xef30);    	// lcddata
	MM_BLOCK_FREE    (0xef50);    	// ram2
	MM_BLOCK_RESERVED(0xf000);    	// motor
	MM_BLOCK_FREE    (0xf010);    	// ram3
	MM_BLOCK_RESERVED(0xfb80);    	// bad Memory and vectors
	MM_BLOCK_FREE    (0xfe00);    	// ram4
	MM_BLOCK_RESERVED(0xff00);    	// stack, onchip

	// expand last block to encompass all available memory
	*current=(int)(((-(int) current)-2)>>1);
	
	mm_update_first_free(&mm_start);
	
#ifdef CONF_TM
	sem_init(&mm_semaphore,0,1);			// init tasksafe lock
#endif
}


//! allocate a block of memory
/*! \param size requested block size
    \return 0 on error, else pointer to block.
*/
void *malloc(size_t size) {
	size_t *ptr,*next;
	
	size=(size+1)>>1;				// only multiples of 2
	
#ifdef CONF_TM
	sem_wait(&mm_semaphore);			// tasksafe
#endif
	ptr=mm_first_free;
	
	while(ptr>=&mm_start) {
		if(*(ptr++)==MM_FREE) {			// free block?
#ifdef CONF_TM
			mm_try_join(ptr);		// unite with later blocks
#endif
			if(*ptr>=size) {		// big enough?
				*(ptr-1)=(size_t)cpid;	// set owner
							
							// split this block?
				if((*ptr-size)>=MM_SPLIT_THRESH) {
					next=ptr+size+1;
					*(next++)=MM_FREE;
					*(next)=*ptr-size-MM_HEADER_SIZE;
					mm_try_join(next);
					
					*ptr=size;
				}
					
							// was it the first free one?
				if(ptr==mm_first_free+1)
					mm_update_first_free(ptr+*ptr+1);
				
#ifdef CONF_TM
				sem_post(&mm_semaphore);
#endif		
				return (void*) (ptr+1);	
			}
		}		
			
		ptr+=(*ptr)+1;				// find next block.
	}
	
#ifdef CONF_TM
	sem_post(&mm_semaphore);
#endif		
	return NULL;
}


//! free a previously allocated block of memory.
/*! \param the_ptr pointer to block

    ever heard of free(software_paradigm)?
*/
void free(void *the_ptr) {
  	size_t *ptr=the_ptr;
#ifndef CONF_TM
      	size_T *p2,*next;
#endif	
	
	if(ptr==NULL || (((size_t)ptr)&1) )
		return;
	
	ptr-=MM_HEADER_SIZE;
	*((size_t*) ptr)=MM_FREE;			// mark as free

#ifdef CONF_TM
	// for task safe operations, free needs to be
	// atomic and nonblocking, because it may be
	// called by the scheduler.
        //
        // therefore, just update mm_first_free
        //
	if(ptr<mm_first_free || mm_first_free<&mm_start)
		mm_first_free=ptr;    		        // update mm_first_free
#else
        // without task management, we have the time to
        // unite neighboring memory blocks.
        //
	p2=&mm_start;
	while(p2!=ptr) {				// we could make free
		next=p2+*(p2+1)+MM_HEADER_SIZE;		// O(1) if we included
		if(*p2==MM_FREE && next==ptr)		// a pointer to the 
			break;				// previous block.
		p2=next;				// I don't want to.
	}
	mm_try_join(p2+1);				// defragment free areas

	if(ptr<mm_first_free || mm_first_free<&mm_start)
		mm_update_first_free(ptr);		// update mm_first_free
#endif
}


//! allocate adjacent blocks of memory
/*! \param nmemb number of blocks
    \param size  individual block size
    \return 0 on error, else pointer to block
*/
void *calloc(size_t nmemb, size_t size) {
	void *ptr;
	
	size*=nmemb;        // FIXME: overflows?
	
	if((ptr=malloc(size))!=NULL)
		memset(ptr,0,size);
		
	return ptr;
}

//! free all blocks allocated by the current process.
/*! called by exit() and kmain().
*/
void mm_reaper() {
	size_t *ptr;
	
	// pass 1: mark as free	
	ptr=&mm_start;
	while(ptr>=&mm_start) {
		if(*ptr==(size_t)cpid)
			*ptr=MM_FREE;
		ptr+=*(ptr+1)+MM_HEADER_SIZE;
	}

	// pass 2: defragment free areas
	// this may alter free blocks
#ifdef CONF_TM
	sem_wait(&mm_semaphore);			// tasksafe
#endif
	ptr=&mm_start;
	while(ptr>=&mm_start) {
		if(*(ptr++)==MM_FREE)
			mm_try_join(ptr);
		ptr+=*ptr+1;
	}
#ifdef CONF_TM
	sem_post(&mm_semaphore);
#endif
}	

//! return the number of bytes of unallocated memory
int mm_free_mem(void) {
	int free = 0;
	size_t *ptr;
	
#ifdef CONF_TM
	sem_wait(&mm_semaphore);
#endif

	// Iterate through the free list
	for (ptr = mm_first_free; 
	     ptr >= &mm_start; 
	     ptr += *(ptr+1) + MM_HEADER_SIZE)
		free += *(ptr+1);

#ifdef CONF_TM
	sem_post(&mm_semaphore);
#endif		
	return free*2;
}

#endif
