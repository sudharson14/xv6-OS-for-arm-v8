
.global trapret

.macro	exception_1_entry
	stp	x29, x30, [sp, #-16]!
	stp	x27, x28, [sp, #-16]!
	stp	x25, x26, [sp, #-16]!
	stp	x23, x24, [sp, #-16]!
	stp	x21, x22, [sp, #-16]!
	stp	x19, x20, [sp, #-16]!
	stp	x17, x18, [sp, #-16]!
	stp	x15, x16, [sp, #-16]!
	stp	x13, x14, [sp, #-16]!
	stp	x11, x12, [sp, #-16]!
	stp	x9, x10, [sp, #-16]!
	stp	x7, x8, [sp, #-16]!
	stp	x5, x6, [sp, #-16]!
	stp	x3, x4, [sp, #-16]!
	stp	x1, x2, [sp, #-16]!
	str	x0, [sp, #-8]!

	add	x21, sp, #248	//SP
	mrs	x22, elr_el1	//LR
	mrs	x23, spsr_el1	//SPSR

	stp	x22, x23, [sp, #-16]!	//LR, SPSR
	str	x21, [sp, #-8]!		//SP
.endm

.macro	exception_1_exit
	ldr	x21, [sp], #8		//SP
	ldp	x22, x23, [sp], #16	//LR, SPSR

	msr	spsr_el1, x23		//SPSR
	msr	elr_el1, x22		//LR

	add	sp, sp, #8		//Leave xo for return value
	ldp	x1, x2, [sp], #16
	ldp	x3, x4, [sp], #16
	ldp	x5, x6, [sp], #16
	ldp	x7, x8, [sp], #16
	ldp	x9, x10, [sp], #16
	ldp	x11, x12, [sp], #16
	ldp	x13, x14, [sp], #16
	ldp	x15, x16, [sp], #16
	ldp	x17, x18, [sp], #16
	ldp	x19, x20, [sp], #16
	ldp	x21, x22, [sp], #16
	ldp	x23, x24, [sp], #16
	ldp	x25, x26, [sp], #16
	ldp	x27, x28, [sp], #16
	ldp	x29, x30, [sp], #16
	eret
.endm

.macro	exception_0_entry
	stp	x29, x30, [sp, #-16]!
	stp	x27, x28, [sp, #-16]!
	stp	x25, x26, [sp, #-16]!
	stp	x23, x24, [sp, #-16]!
	stp	x21, x22, [sp, #-16]!
	stp	x19, x20, [sp, #-16]!
	stp	x17, x18, [sp, #-16]!
	stp	x15, x16, [sp, #-16]!
	stp	x13, x14, [sp, #-16]!
	stp	x11, x12, [sp, #-16]!
	stp	x9, x10, [sp, #-16]!
	stp	x7, x8, [sp, #-16]!
	stp	x5, x6, [sp, #-16]!
	stp	x3, x4, [sp, #-16]!
	stp	x1, x2, [sp, #-16]!
	str	x0, [sp, #-8]!

	mrs	x21, sp_el0	//SP
	mrs	x22, elr_el1	//LR
	mrs	x23, spsr_el1	//SPSR

	stp	x22, x23, [sp, #-16]!	//LR, SPSR
	str	x21, [sp, #-8]!		//SP
.endm

.macro	exception_0_exit
	ldr	x21, [sp], #8		//SP
	ldp	x22, x23, [sp], #16	//LR, SPSR

	msr	spsr_el1, x23		//SPSR
	msr	elr_el1, x22		//LR
	msr	sp_el0, x21		//SP

	ldr	x0, [sp], #8
	ldp	x1, x2, [sp], #16
	ldp	x3, x4, [sp], #16
	ldp	x5, x6, [sp], #16
	ldp	x7, x8, [sp], #16
	ldp	x9, x10, [sp], #16
	ldp	x11, x12, [sp], #16
	ldp	x13, x14, [sp], #16
	ldp	x15, x16, [sp], #16
	ldp	x17, x18, [sp], #16
	ldp	x19, x20, [sp], #16
	ldp	x21, x22, [sp], #16
	ldp	x23, x24, [sp], #16
	ldp	x25, x26, [sp], #16
	ldp	x27, x28, [sp], #16
	ldp	x29, x30, [sp], #16
	eret
.endm

trapret:
    exception_0_exit

/*
 * Exception vectors.
 */
	.align	12
	.globl	vectors
vectors:
	.align	7
	b	_el1_bad_sync	/* Sync EL1t */
	.align	7
	b	_el1_bad_irq	/* IRQ EL1t */
	.align	7
	b	_el1_bad_fiq	/* FIQ EL1t */
	.align	7
	b	_el1_bad_error	/* Error EL1t */

	.align	7
	b	_el1_sync	/* Sync EL1h */
	.align	7
	b	_el1_irq	/* IRQ EL1h */
	.align	7
	b	_el1_fiq	/* FIQ EL1h */
	.align	7
	b	_el1_error	/* Error EL1h */

	.align	7
	b	_el0_sync	/* Sync EL0 */
	.align	7
	b	_el0_irq	/* IRQ EL0 */
	.align	7
	b	_el0_fiq	/* FIQ EL0 */
	.align	7
	b	_el0_error	/* Error EL0 */

	.align	6
_el1_bad_sync:
	exception_1_entry
	mov	x0, sp
	mov	x1, #1
	mrs	x2, esr_el1
	bl	bad_handler
	b	.

	.align	6
_el1_bad_irq:
	exception_1_entry
	mov	x0, sp
	mov	x1, #2
	mrs	x2, esr_el1
	bl	bad_handler
	b	.

	.align	6
_el1_bad_fiq:
	exception_1_entry
	mov	x0, sp
	mov	x1, #3
	mrs	x2, esr_el1
	bl	bad_handler
	b	.

	.align	6
_el1_bad_error:
	exception_1_entry
	mov	x0, sp
	mov	x1, #4
	mrs	x2, esr_el1
	bl	bad_handler
	b	.


	.align	6
_el1_sync:
	exception_1_entry
	mrs	x2, esr_el1
	lsr	x24, x2, #26
	cmp	x24, #0x25
	b.eq	el1_da
	cmp	x24, #0x21
	b.eq	el1_ia
	b	el1_default

el1_da:
	mov	x0, sp
	mov	x1, #1
	bl	dabort_handler
	b	.

el1_ia:
	mov	x0, sp
	mov	x1, #1
	bl	iabort_handler
	b	.

el1_default:
	mov	x0, sp
	mov	x1, #1
	bl	default_handler
	b	.

	.align	6
_el1_irq:
	exception_1_entry
	mov	x0, sp
	mov	x1, #1
	mrs	x2, esr_el1
	bl	irq_handler
	exception_1_exit

	.align	6
_el1_fiq:
	exception_1_entry
	mov	x0, sp
	mov	x1, #1
	mrs	x2, esr_el1
	bl	fiq_handler
	b	.

	.align	6
_el1_error:
	exception_1_entry
	mov	x0, sp
	mov	x1, #1
	mrs	x2, esr_el1
	bl	error_handler
	b	.


	.align	6
_el0_sync:
	exception_0_entry
	mrs	x2, esr_el1
	lsr	x24, x2, #26
	cmp	x24, #0x15
	b.eq	el0_svc
	cmp	x24, #0x24
	b.eq	el0_da
	cmp	x24, #0x20
	b.eq	el0_ia
	cmp	x24, #0x00
	b.eq	el0_undef
	b	el0_default

el0_svc:
	mov	x0, sp
	mov	x1, #0
	bl	swi_handler
	exception_0_exit

el0_da:
	mov	x0, sp
	mov	x1, #0
	bl	dabort_handler
	b	.

el0_ia:
	mov	x0, sp
	mov	x1, #0
	bl	iabort_handler
	b	.

el0_undef:
	mov	x0, sp
	mov	x1, #0
	bl	und_handler
	b	.

el0_default:
	mov	x0, sp
	mov	x1, #0
	bl	default_handler
	b	.

	.align	6
_el0_irq:
	exception_0_entry
	mov	x0, sp
	mov	x1, #0
	mrs	x2, esr_el1
	bl	irq_handler
	exception_0_exit

	.align	6
_el0_fiq:
	exception_0_entry
	mov	x0, sp
	mov	x1, #0
	mrs	x2, esr_el1
	bl	fiq_handler
	b	.

	.align	6
_el0_error:
	exception_0_entry
	mov	x0, sp
	mov	x1, #0
	mrs	x2, esr_el1
	bl	error_handler
	b	.


