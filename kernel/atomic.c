#include "config.h"
#include <atomic.h>

#ifdef CONF_ATOMIC
/**
 * increment atomic counter without interruption.
 * locks interrupts except NMI, increments count
 * then restores interrupts.
 * @param counter the counter resource to be incremented
 * @see atomic_dec
 */
void atomic_inc(atomic_t* counter);
__asm__("\n\
.text\n\
.globl _atomic_inc\n\
       _atomic_inc:\n\
	stc   ccr, r1h				; save flags\n\
	orc   #0x80, ccr			; disable all but NMI\n\
	mov.b @r0, r1l\n\
	inc   r1l\n\
	mov.b r1l, @r0\n\
	ldc   r1h, ccr				; restore flags\n\
	rts\n\
");

/**
 * decrement atomic counter without interruption.
 * locks interrupts except NMI, decrements count
 * then restores interrupts.
 * @param counter the counter resource to be decremented
 * @see atomic_inc
 */
void atomic_dec(atomic_t* counter);
__asm__("\n\
.text\n\
.globl _atomic_dec\n\
       _atomic_dec:\n\
         stc   ccr, r1h\n\
         orc   #0x80, ccr\n\
         mov.b @r0, r1l\n\
         dec   r1l\n\
         mov.b r1l, @r0\n\
         ldc   r1h, ccr\n\
         rts\n\
");
#endif
