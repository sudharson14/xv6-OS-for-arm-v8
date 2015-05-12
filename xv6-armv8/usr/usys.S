#include "syscall.h"

#define SYSCALL(name) \
.globl name; \
name: \
	STR x4, [sp, #-0x08]!;\
	MOV x4, x3;\
	MOV x3, x2;\
	MOV x2, x1;\
	MOV x1, x0;\
	MOV x0, #SYS_ ## name;\
	SVC 0x00;\
	LDR x4, [sp], #0x08;\
	br x30;	//lr = x30

SYSCALL(fork)
SYSCALL(exit)
SYSCALL(wait)
SYSCALL(pipe)
SYSCALL(read)
SYSCALL(write)
SYSCALL(close)
SYSCALL(kill)
SYSCALL(exec)
SYSCALL(open)
SYSCALL(mknod)
SYSCALL(unlink)
SYSCALL(fstat)
SYSCALL(link)
SYSCALL(mkdir)
SYSCALL(chdir)
SYSCALL(dup)
SYSCALL(getpid)
SYSCALL(sbrk)
SYSCALL(sleep)
SYSCALL(uptime)
