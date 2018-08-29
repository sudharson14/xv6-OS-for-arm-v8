// The ARM UART, a memory mapped device
#include "types.h"
#include "defs.h"
#include "param.h"
#include "arm.h"
#include "proc.h"

// trap routine
void handle_user_events(struct trapframe *r, uint32 el, uint32 esr) {
    
    if ( ( r->spsr & 0xf ) != 0 )
	    panic("Invalid Saved Processor State.");

    if ( proc->killed != 0 ) {
	    
	cprintf("proc killed: pid: %d name: %s EL:%d ESR:0x%x\n",
	    proc->pid, proc->name, el, esr);
	exit();
    }
}

// trap routine
void swi_handler (struct trapframe *r, uint32 el, uint32 esr)
{
    //cprintf("\tswi_handler: %d\n", r->r0);
    proc->tf = r;
    syscall ();
}

// trap routine
void irq_handler (struct trapframe *r, uint32 el, uint32 esr)
{
    // proc points to the current process. If the kernel is
    // running scheduler, proc is NULL.
    if (proc != NULL) {
        proc->tf = r;
    }

    pic_dispatch (r);
}

// trap routine
void dabort_handler (struct trapframe *r, uint32 el, uint32 esr)
{
    uint64 fa;
    extern void show_callstk (char *s);
    cli();

    // read the fault address register
    asm("MRS %[r], FAR_EL1": [r]"=r" (fa)::);
    if ( ( r->spsr & 0xf ) == 0 ) {

	cprintf("Data abort  esr %x on cpu %d pc 0x%x addr 0x%x -- kill proc\n",
	    esr, cpu->id, r->pc, fa);
	kill(proc->pid);
    } else {

	cprintf ("data abort: instruction 0x%x, fault addr 0x%x\n",
	    r->pc, fa);
	//show_callstk("Stack dump for data exception.");
    }
}

// trap routine
void iabort_handler (struct trapframe *r, uint32 el, uint32 esr)
{
    uint64 fa;

    cli();

    // read the fault address register
    asm("MRS %[r], FAR_EL1": [r]"=r" (fa)::);
    if ( ( r->spsr & 0xf ) == 0 ) {

	cprintf("Instruction abort esr %x on cpu %d pc 0x%x addr 0x%x -- kill proc\n",
	    esr, cpu->id, r->pc, fa);
	kill(proc->pid);
    } else {

	cprintf ("prefetch abort at: 0x%x\n", r->pc);
	dump_trapframe (r);
    }
}

// trap routine
void reset_handler (struct trapframe *r, uint32 el, uint32 esr)
{
    cli();
    cprintf ("reset at: 0x%x \n", r->pc);
}

// trap routine
void und_handler (struct trapframe *r, uint32 el, uint32 esr)
{
    cli();
    if ( ( r->spsr & 0xf ) == 0 ) {
	
	cprintf("Undefined trap  esr %x on cpu %d pc 0x%x -- kill proc\n", 
	    esr, cpu->id, r->pc);
	kill(proc->pid);
    } else {

	cprintf ("und at: 0x%x \n", r->pc);
	dump_trapframe (r);
    }
}

// trap routine
void na_handler (struct trapframe *r, uint32 el, uint32 esr)
{
    cli();
    cprintf ("n/a at: 0x%x \n", r->pc);
}

// trap routine
void fiq_handler (struct trapframe *r, uint32 el, uint32 esr)
{
    cli();
    cprintf ("fiq at: 0x%x \n", r->pc);
}

// trap routine
void bad_handler (struct trapframe *r, uint32 el, uint32 esr)
{
    cli();
    cprintf ("Bad Exception\n");
}

// trap routine
void error_handler (struct trapframe *r, uint32 el, uint32 esr)
{
    cli();
    cprintf ("Error Exception\n");
}

// trap routine
void default_handler (struct trapframe *r, uint32 el, uint32 esr)
{
    cli();
    cprintf ("Default Exception\n");
}

// low-level init code: in real hardware, lower memory is usually mapped
// to flash during startup, we need to remap it to SDRAM
void trap_init ( )
{
    //Nothing to do
}

void dump_trapframe (struct trapframe *tf)
{
    cprintf ("     sp: 0x%x\n", tf->sp);
    cprintf ("     pc: 0x%x\n", tf->pc);
    cprintf ("   spsr: 0x%x\n", tf->spsr);
    cprintf ("     r0: 0x%x\n", tf->r0);
    cprintf ("     r1: 0x%x\n", tf->r1);
    cprintf ("     r2: 0x%x\n", tf->r2);
    cprintf ("     r3: 0x%x\n", tf->r3);
    cprintf ("     r4: 0x%x\n", tf->r4);
    cprintf ("     r5: 0x%x\n", tf->r5);
    cprintf ("     r6: 0x%x\n", tf->r6);
    cprintf ("     r7: 0x%x\n", tf->r7);
    cprintf ("     r8: 0x%x\n", tf->r8);
    cprintf ("     r9: 0x%x\n", tf->r9);
    cprintf ("    r10: 0x%x\n", tf->r10);
    cprintf ("    r11: 0x%x\n", tf->r11);
    cprintf ("    r12: 0x%x\n", tf->r12);
    cprintf ("    r13: 0x%x\n", tf->r13);
    cprintf ("    r14: 0x%x\n", tf->r14);
    cprintf ("    r15: 0x%x\n", tf->r15);
    cprintf ("    r16: 0x%x\n", tf->r16);
    cprintf ("    r17: 0x%x\n", tf->r17);
    cprintf ("    r18: 0x%x\n", tf->r18);
    cprintf ("    r19: 0x%x\n", tf->r19);
    cprintf ("    r20: 0x%x\n", tf->r20);
    cprintf ("    r21: 0x%x\n", tf->r21);
    cprintf ("    r22: 0x%x\n", tf->r22);
    cprintf ("    r23: 0x%x\n", tf->r23);
    cprintf ("    r24: 0x%x\n", tf->r24);
    cprintf ("    r25: 0x%x\n", tf->r25);
    cprintf ("    r26: 0x%x\n", tf->r26);
    cprintf ("    r27: 0x%x\n", tf->r27);
    cprintf ("    r28: 0x%x\n", tf->r28);
    cprintf ("    r29: 0x%x\n", tf->r29);
    cprintf ("    r30: 0x%x\n", tf->r30);
}
