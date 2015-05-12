// Support of ARM PrimeCell Vectored Interrrupt Controller (PL190)
#include "types.h"
#include "defs.h"
#include "param.h"
#include "arm.h"
#include "memlayout.h"
#include "mmu.h"

// PL190 supports the vectored interrupts and non-vectored interrupts.
// In this code, we use non-vected interrupts (aka. simple interrupt).
// The flow to handle simple interrupts is as the following:
//		1. an interrupt (IRQ) occurs, trap.c branches to our IRQ handler
//		2. read the VICIRQStatus register, for each source generating
//		   the interrrupt:
//			2.1 locate the correct ISR
//			2.2 execute the ISR
//			2.3 clear the interrupt
//		3 return to trap.c, which will resume interrupted routines
// Note: must not read VICVectorAddr


// define the register offsets (in the unit of 4 bytes). The base address
// of the VIC depends on the board
static volatile uint* vic_base;

#define VIC_IRQSTATUS	0 // status of interrupts after masking by ENABLE and SEL
#define VIC_FIQSTATUS	1 // status of interrupts after masking
#define VIC_RAWINTR		2 // status of interrupts before masking
#define VIC_INTSEL		3 // interrupt select (IRQ or FIQ), by default IRQ
#define	VIC_INTENABLE	4 // enable interrupts (1 - enabled, 0 - disabled)
#define VIC_INTCLEAR	5 // clear bits in ENABLE register (1 - clear it)
#define VIC_PROTECTIOIN	8 // who can access: user or privileged

#define NUM_INTSRC		32 // numbers of interrupt source supported

static ISR isrs[NUM_INTSRC];

static void default_isr (struct trapframe *tf, int n)
{
    cprintf ("unhandled interrupt: %d\n", n);
}

// initialize the PL190 VIC
void pic_init (void * base)
{
    int i;

    // set the base for the controller and disable all interrupts
    vic_base = base;
    vic_base[VIC_INTCLEAR] = 0xFFFFFFFF;

    for (i = 0; i < NUM_INTSRC; i++) {
        isrs[i] = default_isr;
    }
}

// enable an interrupt (with the ISR)
void pic_enable (int n, ISR isr)
{
    if ((n<0) || (n >= NUM_INTSRC)) {
        panic ("invalid interrupt source");
    }

    // write 1 bit enable the interrupt, 0 bit has no effect
    isrs[n] = isr;
    vic_base[VIC_INTENABLE] = (1 << n);
}

// disable an interrupt
void pic_disable (int n)
{
    if ((n<0) || (n >= NUM_INTSRC)) {
        panic ("invalid interrupt source");
    }

    vic_base[VIC_INTCLEAR] = (1 << n);
    isrs[n] = default_isr;
}

// dispatch the interrupt
void pic_dispatch (struct trapframe *tp)
{
    uint intstatus;
    int		i;

    intstatus = vic_base[VIC_IRQSTATUS];

    for (i = 0; i < NUM_INTSRC; i++) {
        if (intstatus & (1<<i)) {
            isrs[i](tp, i);
        }
    }

    intstatus = vic_base[VIC_IRQSTATUS];
}

