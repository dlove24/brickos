/*! \file   c++/stub.c
    \brief  A collection of dirty hacks to get C++ compilation going.
    \author Markus L. Noga <markus@noga.de>
    
    This stub provides the necessary runtime environment for basic C++.
    The following features currently work:
    <ul><li>Classes</li>
	<li>Inheritance</li>
	<li>new and delete</li>
	<li>Virtual methods and destructors</li>
    </ul>
    
    Several features are still amiss:
    <ul><li>Throwing an exception will abort the current thread.</li>
        <li>RTTI is unimplemented.</li>
    </ul>
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
 *                  Lars Berntzon <lasse@cecilia-data.se>
 */

typedef unsigned size_t;
extern void exit(int) __attribute__ ((noreturn));
extern void* malloc(size_t);
extern void free(void*);

//! Abort execution
/*! Functional version, calls exit(1)
*/
void __terminate() {
  exit(-1);
}

//! Throw an execption in a setjmp environment
/*! FIXME: this just terminates the task.
*/
void __sjthrow() {
  __terminate();
}

//! Return current exception handler context
/*! FIXME: null pointer returned
*/
void* __get_eh_context() {
  return (void*) 0;
}
//! FIXME: rtti nop hack.
int __rtti_si() {
  return 0;
}
//! FIXME: rtti nop hack.
int __rtti_user() {
  return 0;
}

//! builtin delete function
/*! frees the memory block
    \param ptr the block to free
*/
void __builtin_delete(void* ptr) {
  free(ptr);
}

//! builtin new function
/*! creates a memory block for an object.
    \param size size of memory to allocate
*/
void *__builtin_new(unsigned int size) {
  return malloc(size);
}    

void
__pure_virtual ()
{
  __terminate ();
}

