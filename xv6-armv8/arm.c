// BSP support routine
#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "proc.h"
#include "arm.h"
#include "mmu.h"

void cli (void)
{
    asm("MSR DAIFSET, #2":::);
}

void sti (void)
{
    asm("MSR DAIFCLR, #2":::);
}

// return whether interrupt is currently enabled
int int_enabled ()
{
    uint32 val;

    asm("MRS %[v], DAIF": [v]"=r" (val)::);

    return !(val & DIS_INT);
}

// Pushcli/popcli are like cli/sti except that they are matched:
// it takes two popcli to undo two pushcli.  Also, if interrupts
// are off, then pushcli, popcli leaves them off.

void pushcli (void)
{
    int enabled;

    enabled = int_enabled();

    cli();

    if (cpu->ncli++ == 0) {
        cpu->intena = enabled;
    }
}

void popcli (void)
{
    if (int_enabled()) {
        panic("popcli - interruptible");
    }

    if (--cpu->ncli < 0) {
        cprintf("cpu (%d)->ncli: %d\n", cpu, cpu->ncli);
        panic("popcli -- ncli < 0");
    }

    if ((cpu->ncli == 0) && cpu->intena) {
        sti();
    }
}

// Record the current call stack in pcs[] by following the call chain.
// In ARM ABI, the function prologue is as:
//		push	{fp, lr}
//		add		fp, sp, #4
// so, fp points to lr, the return address
void getcallerpcs (void * v, uint64 pcs[])
{
    uint64 *fp;
    int i;

    fp = (uint64*) v;

    for (i = 0; i < N_CALLSTK; i++) {
        if ((fp == 0) || (fp < (uint64*) KERNBASE) || (fp == (uint64*) 0xffffffff)) {
            break;
        }

        fp = fp - 1;			// points fp to the saved fp
        pcs[i] = fp[1];     // saved lr
        fp = (uint64*) fp[0];	// saved fp
    }

    for (; i < N_CALLSTK; i++) {
        pcs[i] = 0;
    }
}

void show_callstk (char *s)
{
    int i;
    uint64 fp;
    uint64 pcs[N_CALLSTK];

    cprintf("%s\n", s);

    asm("MOV %[r], x29":[r]"=r" (fp): :);

    getcallerpcs((void *)fp, pcs);

    for (i = N_CALLSTK - 1; i >= 0; i--) {
        cprintf("%d: 0x%x\n", i + 1, pcs[i]);
    }

}
