#include "arm.h"
#include "memlayout.h"

.text
.align 16
.global _start

_start:
	mrs   x0, CurrentEL
	and   x0, x0, #0x0C  //  Am I in EL3?
	cmp   x0, #0xc
	b.eq  drop_el2
	cmp   x0, #0x8       //  Am I in EL2?
	b.eq  drop_el1
	b     start_el1
drop_el2:
	// Drop into EL2 from EL3
	// SCR_EL3_NS(0x1) | SCR_EL3_RES(0x3<<4) | SCR_EL3_SMA(1<<7) | SCR_EL3_RW(0x1<<10)
	mov   x0, #( (0x1) | ( 0x3 << 4 ) | ( 0x1 << 7 ) | ( 0x1 << 10 ) )
	msr   scr_el3, x0
	adr   x0, start_el2
	msr   elr_el3, x0
	// SPSR_FROM_AARCH64(0) | SPSR_MODE_EL2(0x8) | SPSR_SP_SEL_N(0x1)
	mov   x0, #( 0x8 | 0x1 )
	msr   spsr_el3, x0
	eret
start_el2:
drop_el1:
	// Drop into EL1 from EL2
	// HCR_EL2_RW ( 0x1 << 31 ) 
	mov   x0, #( 0x1 << 31 )
	// HCR_EL2_SWIO ( 0x1 << 1 )
	orr   x0, x0,#( 0x1 << 1 ) 
	msr   hcr_el2, x0
	adr   x0, start_el1
	msr   elr_el2, x0
	// SPSR_DAIF_D_BIT ( 0x1 << 9 ) | SPSR_DAIF_A_BIT ( 0x1 << 8 ) |
	// SPSR_DAIF_I_BIT ( 0x1 << 7 ) | SPSR_DAIF_F_BIT ( 0x1 << 6 ) |
	// SPSR_FROM_AARCH64 (0x0) | SPSR_MODE_EL1(0x4) | SPSR_SP_SEL_N (0x1)
	mov   x0, #( ( 0x1 << 9 ) | ( 0x1 << 8 ) | ( 0x1 << 7 ) | ( 0x1 << 6 ) | (0x0) | ( 0x4 ) | (0x1) )
	msr   spsr_el2, x0
	eret

start_el1:	
	# initialize stack pointers for svc modes
	mov     x0, #1     // select SP_EL1
	msr     spsel, x0
	isb

	adrp    x0, init_stktop
	mov     sp, x0

	# clear the entry bss section, the svc stack, and kernel page table
	LDR     x1, =edata_entry
	LDR     x2, =end_entry
	MOV     x3, #0x00
	# MOV   x3, #0xab  # paint tags on stack for identify problems in qemu

1:
	CMP     x1, x2
	B.GT    2f
	STR     x3, [x1]
	ADD     x1, x1, #0x08
	BLT     1b
2:

	BL      start
	B .


# during startup, kernel stack uses user address, now switch it to kernel addr
.global jump_stack
jump_stack:
	MOV     x0, sp
	LDR     x1, =KERNBASE
	ADD     sp, x1, x0
	RET
