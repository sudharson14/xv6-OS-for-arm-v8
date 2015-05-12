// Mutual exclusion spin locks.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "arm.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"

void initlock(struct spinlock *lk, char *name)
{
    lk->name = name;
    lk->locked = 0;
    lk->cpu = 0;
}

// For single CPU systems, there is no need for spinlock.
// Add the support when multi-processor is supported.


// Acquire the lock.
// Loops (spins) until the lock is acquired.
// Holding a lock for a long time may cause
// other CPUs to waste time spinning to acquire it.
void acquire(struct spinlock *lk)
{
    pushcli();		// disable interrupts to avoid deadlock.
    lk->locked = 1;	// set the lock status to make the kernel happy

#if 0
    if(holding(lk))
        panic("acquire");

    // The xchg is atomic.
    // It also serializes, so that reads after acquire are not
    // reordered before it.
    while(xchg(&lk->locked, 1) != 0)
        ;

    // Record info about lock acquisition for debugging.
    lk->cpu = cpu;
    getcallerpcs(get_fp(), lk->pcs);

#endif
}

// Release the lock.
void release(struct spinlock *lk)
{
#if 0
    if(!holding(lk))
        panic("release");

    lk->pcs[0] = 0;
    lk->cpu = 0;

    // The xchg serializes, so that reads before release are
    // not reordered after it.  The 1996 PentiumPro manual (Volume 3,
    // 7.2) says reads can be carried out speculatively and in
    // any order, which implies we need to serialize here.
    // But the 2007 Intel 64 Architecture Memory Ordering White
    // Paper says that Intel 64 and IA-32 will not move a load
    // after a store. So lock->locked = 0 would work here.
    // The xchg being asm volatile ensures gcc emits it after
    // the above assignments (and after the critical section).
    xchg(&lk->locked, 0);
#endif

    lk->locked = 0; // set the lock state to keep the kernel happy
    popcli();
}


// Check whether this cpu is holding the lock.
int holding(struct spinlock *lock)
{
    return lock->locked; // && lock->cpu == cpus;
}

