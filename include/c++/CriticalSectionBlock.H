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
// Contributor Henner Zeller <H.Zeller@acm.org>

#ifndef __CRITICALSECTIONBLOCK_H
#define __CRITICALSECTIONBLOCK_H

#include <critsec.h>

/**
 * A CriticalSectionBlock locks a critical section for the lifetime of the
 * CriticalSectionBlock.
 *
 * The CriticalSectionBlock gets a critical section and initially 
 * enters it, i.e. the constructor blocks until the section is free. The
 * critical section is unlocked in the destructor. 
 *
 * The actual trick is, that if the CriticalSectionBlock is created
 * as automatic variable on the stack (complicated expression for
 * <em>local variable</em>), C++ implicitly calls the destructor, 
 * whenever we leave the block it is created in.
 *
 * Leaving the block may be due to a regular return, or an Exception or
 * just falling off the edge, thus this automatically takes care of 
 * cleaning up the section which is especially handy if we expect Exceptions.
 * Never forgotten-to-unlock critical sections again .. (isn't this cool ?)
 *
 * Usage like:
 * <pre>
 * --------------------------------------------------------------------
 * critsec_t someCriticalSection;
 * void foo () {
 *     CriticalSectionBlock criticalSection(&someCriticalSection);
 *     //.. critical section protected ...
 *     myCriticalDataStructure = 42;
 *    // criticalSection is automatically released
 * }
 * --------------------------------------------------------------------- 
 * </pre>
 *
 * There is as well a macro available which utilizes this class to make
 * it <em>really</em> easy to use and to read. People who have programmed
 * Java reckognize this; use this in favour of the plain CriticalSectionBlock
 * because it improves readability a lot:
 *
 * <pre>
 * --------------------------------------------------------------------- 
 * critsec_t someCriticalSection;
 * void foo () {
 *     // ...
 *     synchronized (&someCriticalSection) {
 *         //.. critical section protected...
 *         myCriticalDataStructure = 42;
 *     }
 *     // ...
 * }
 * --------------------------------------------------------------------- 
 * </pre>
 *
 * @author Henner Zeller <H.Zeller@acm.org>
 */
class CriticalSectionBlock {
private:
	critsec_t *_critsec;
	/**
	 * little hack, to make it possible to implement the synchronized()
	 * macro with the for(;;) loop below.
	 */
	char   _checkonceHackCounter;

public:
	/**
	 * Constructor locks the critical section.
	 */
	CriticalSectionBlock (critsec_t *critsec) 
		: _critsec(critsec),
		  _checkonceHackCounter(1)
	{ 
		enter_critical_section(_critsec);
	}
	
	/**
	 * don't use. Used internally for the synchronized() { } macro.
	 */
	inline bool checkonce() { return _checkonceHackCounter-- > 0; }

	/**
	 * destructor automatically unlocks the critical section.
	 */
	~CriticalSectionBlock () {
		leave_critical_section(_critsec);
	}
};

/**
 * cool macro which makes it possible to use the CriticalSectionBlock the
 * way you are used to in Java (..if you are used to java). Basically you
 * use it this way:
 *
 * <pre>
 * --------------------------------------------------------------------- 
 * critsec_t someCriticalSection;
 * void foo () {
 *     // ...
 *     synchronized (&someCriticalSection) {
 *         //.. critical section protected with critical section.
 *         myCriticalDataStructure = 42;
 *     }
 *     // ...
 * }
 * --------------------------------------------------------------------- 
 * </pre>
 *
 * The implementation of this macro creates a automatic variable within the
 * initialization part of a for-loop which is valid for the lifetime of the
 * block within the curly braces.
 *
 * BUGS: wastes some CPU cyles for a counter which is neccessary to abuse
 * the for(;;) loop to have this behaviour. Any ideas ?
 *
 * @author Henner Zeller <H.Zeller@acm.org>
 */
#define synchronized(cs) for(CriticalSectionBlock __currentlocked(cs);__currentlocked.checkonce();/* */)

/* 
 * Local variables:
 * c-basic-offset: 8
 * End:
 */

#endif /* __CRITICALSECTIONBLOCK_H */
