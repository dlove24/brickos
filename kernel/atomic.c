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
__asm__("
.text
.globl _atomic_inc
       _atomic_inc:
	stc   ccr, r1h				; save flags
	orc   #0x80, ccr			; disable all but NMI
	mov.b @r0, r1l
	inc   r1l
	mov.b r1l, @r0
	ldc   r1h, ccr				; restore flags
	rts
");

/**
 * decrement atomic counter without interruption.
 * locks interrupts except NMI, decrements count
 * then restores interrupts.
 * @param counter the counter resource to be decremented
 * @see atomic_inc
 */
void atomic_dec(atomic_t* counter);
__asm__("
.text
.globl _atomic_dec
       _atomic_dec:
         stc   ccr, r1h
         orc   #0x80, ccr
         mov.b @r0, r1l
         dec   r1l
         mov.b r1l, @r0
         ldc   r1h, ccr
         rts
");
#endif
