#include "arm.h"
#include "memlayout.h"

.text
.align 16
.global _start

_start:

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
