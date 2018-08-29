// BSP support routine
#include "types.h"
#include "defs.h"
#include "param.h"
#include "arm.h"
#include "proc.h"
#include "memlayout.h"
#include "mmu.h"

extern void* end;

struct cpu	cpus[NCPU];
struct cpu	*cpu;

#define MB (1024*1024)

void kmain (void)
{
    cpu = &cpus[0];

    uart_init (P2V(UART0));

    init_vmm ();
    kpt_freerange (align_up(&end, PT_SZ), P2V_WO(INIT_KERNMAP));
    paging_init (INIT_KERNMAP, PHYSTOP);

    kmem_init ();
    kmem_init2(P2V(INIT_KERNMAP), P2V(PHYSTOP));

    trap_init ();				// vector table and stacks for models
   
    gic_init(P2V(VIC_BASE));			// arm v2 gic init
    uart_enable_rx ();				// interrupt for uart
    consoleinit ();				// console
    pinit ();					// process (locks)

    binit ();					// buffer cache
    fileinit ();				// file table
    iinit ();					// inode cache
    ideinit ();					// ide (memory block device)

    timer_init (HZ);				// the timer (ticker)

    sti ();
    userinit();					// first user process
    scheduler();				// start running processes
}
