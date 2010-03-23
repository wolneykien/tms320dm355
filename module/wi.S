/*
 *  Copyright (C) 2010 Paul Wolneykien
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/linkage.h>
#include <asm/assembler.h>
#include <asm/param.h>
		.text

/*
 * cpu_arm926_do_idle()
 *
 * Called with IRQs disabled
 */
	.align	10
ENTRY(__wait_for_interrupt)
	mov	r0, #0
	mrc	p15, 0, r1, c1, c0, 0		@ Read control register
	mcr	p15, 0, r0, c7, c10, 4		@ Drain write buffer
	bic	r2, r1, #1 << 12
	mrs	r3, cpsr			@ Disable FIQs while Icache
	orr	ip, r3, #PSR_F_BIT		@ is disabled
	msr	cpsr_c, ip
	mcr	p15, 0, r2, c1, c0, 0		@ Disable I cache
	mcr	p15, 0, r0, c7, c0, 4		@ Wait for interrupt
	mcr	p15, 0, r1, c1, c0, 0		@ Restore ICache enable
	msr	cpsr_c, r3			@ Restore FIQ state
	mov	pc, lr
ENDPROC(__wait_for_interrupt)
