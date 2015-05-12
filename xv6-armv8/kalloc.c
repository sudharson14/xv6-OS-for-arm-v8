// Physical memory allocator, intended to allocate
// memory for user processes, kernel stacks, page table pages,
// and pipe buffers. Allocates 4096-byte pages.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "spinlock.h"
#include "arm.h"

void freerange(void *vstart, void *vend);
extern char end[]; // first address after kernel loaded from ELF file

struct run {
    struct run *next;
};

static struct {
    struct spinlock lock;
    int use_lock;
    struct run *freelist;
} kmem;

void kmem_init (void)
{
    initlock(&kmem.lock, "kmem");
    kmem.use_lock = 0;
}

// Initialization happens in two phases.
// 1. main() calls kinit1() while still using entrypgdir to place just
// the pages mapped by entrypgdir on free list.
// 2. main() calls kinit2() with the rest of the physical pages
// after installing a full page table that maps them on all cores.
void kinit1(void *vstart, void *vend)
{
    freerange(vstart, vend);
}

void kinit2(void *vstart, void *vend)
{
    freerange(vstart, vend);
    kmem.use_lock = 1;
}

void freerange(void *vstart, void *vend)
{
    char *p;

    p = (char*)align_up (vstart, PTE_SZ);

    for(; p + PTE_SZ <= (char*)vend; p += PTE_SZ) {
        kfree(p);
    }
}

//PAGEBREAK: 21
// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void kfree(char *v)
{
    struct run *r;

    if((uint)v % PTE_SZ || v < end || v2p(v) >= PHYSTOP) {
        cprintf("kfree(0x%x)\n", v);
        panic("kfree");
    }

    // Fill with junk to catch dangling refs.
    //memset(v, 0x00, PG_SIZE);

    if(kmem.use_lock) {
        acquire(&kmem.lock);
    }

    r = (struct run*)v;
    r->next = kmem.freelist;
    kmem.freelist = r;

    if(kmem.use_lock) {
        release(&kmem.lock);
    }
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
char* kalloc(void)
{
    struct run *r;

    if(kmem.use_lock) {
        acquire(&kmem.lock);
    }

    r = kmem.freelist;

    if(r) {
        kmem.freelist = r->next;
    }

    if(kmem.use_lock) {
        release(&kmem.lock);
    }

    return (char*)r;
}
