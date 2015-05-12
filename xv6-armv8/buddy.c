// Buddy memory allocator
#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "spinlock.h"
#include "arm.h"


// this file implement the buddy memory allocator. Each order divides
// the memory pool into equal-sized blocks (2^n). We use bitmap to record
// allocation status for each block. This allows for efficient merging
// when blocks are freed. We also use double-linked list to chain together
// free blocks (for each order), thus allowing fast allocation. There is
// about 8% overhead (maximum) for this structure.

#define MAX_ORD      12
#define MIN_ORD      6
#define N_ORD        (MAX_ORD - MIN_ORD +1)

struct mark {
    uint32  lnks;       // double links (actually indexes) 
    uint32  bitmap;     // bitmap, whether the block is available (1=available)
};

// lnks is a combination of previous link (index) and next link (index)
#define PRE_LNK(lnks)   ((lnks) >> 16)
#define NEXT_LNK(lnks)  ((lnks) & 0xFFFF)
#define LNKS(pre, next) (((pre) << 16) | ((next) & 0xFFFF))
#define NIL             ((uint16)0xFFFF)

struct order {
    uint32  head;       // the first non-empty mark
    uint32  offset;     // the first mark
};

struct kmem {
    struct spinlock lock;
    uint64            start;             // start of memory for marks
    uint64            start_heap;        // start of allocatable memory
    uint64            end;
    struct order    orders[N_ORD];  // orders used for buddy systems
};

static struct kmem kmem;

// coversion between block id to mark and memory address
static inline struct mark* get_mark (int order, int idx)
{
    return (struct mark*)kmem.start + (kmem.orders[order - MIN_ORD].offset + idx);
}

static inline void* blkid2mem (int order, int blkid)
{
    return (void*)(kmem.start_heap + (1 << order) * blkid);
}

static inline int mem2blkid (int order, void *mem)
{
    return ((uint64)mem - kmem.start_heap) >> order;
}

static inline int available (uint bitmap, int blk_id)
{
    return bitmap & (1 << (blk_id & 0x1F));
}

void kmem_init (void)
{
    initlock(&kmem.lock, "kmem");
}

void kmem_init2(void *vstart, void *vend)
{
    long            i, j;
    uint64          total, n;
    uint64          len;
    struct order    *ord;
    struct mark     *mk;
    
    kmem.start = (uint64)vstart;
    kmem.end   = (uint64)vend;
    len = kmem.end - kmem.start;

    // reserved memory at vstart for an array of marks (for all the orders)
    n = (len >> (MAX_ORD + 5)) + 1; // estimated # of marks for max order
    total = 0;
    
    for (i = N_ORD - 1; i >= 0; i--) {
        ord = kmem.orders + i;
        ord->offset = total;
        ord->head = NIL;
        
        // set the bitmaps to mark all blocks not available
        for (j = 0; j < n; j++) {
            mk = get_mark(i + MIN_ORD, j);
            mk->lnks = LNKS(NIL, NIL);
            mk->bitmap = 0;
        }

        total += n;
        n <<= 1;     // each order doubles required marks
    }

    // add all available memory to the highest order bucket
    kmem.start_heap = align_up(kmem.start + total * sizeof(*mk), 1 << MAX_ORD);
    
    for (i = kmem.start_heap; i < kmem.end; i += (1 << MAX_ORD)){
        kfree ((void*)i, MAX_ORD);
    }
}

// mark a block as unavailable
static void unmark_blk (int order, int blk_id)
{
    struct mark     *mk, *p;
    struct order    *ord;
    int             prev, next;

    ord = &kmem.orders[order - MIN_ORD];
    mk  = get_mark (order, blk_id >> 5);

    // clear the bit in the bitmap
    if (!available(mk->bitmap, blk_id)) {
        panic ("double alloc\n");
    }

    mk->bitmap &= ~(1 << (blk_id & 0x1F));
    
    // if it's the last block in the bitmap, delete from the list
    if (mk->bitmap == 0) {
        blk_id >>= 5;
        
        prev = PRE_LNK(mk->lnks);
        next = NEXT_LNK(mk->lnks);

        if (prev != NIL) {
            p = get_mark(order, prev);
            p->lnks = LNKS(PRE_LNK(p->lnks), next);
            
        } else if (ord->head == blk_id) {
            // if we are the first in the link
            ord->head = next;
        }

        if (next != NIL) {
            p = get_mark(order, next);
            p->lnks = LNKS(prev, NEXT_LNK(p->lnks));
        }

        mk->lnks = LNKS(NIL, NIL);
    }
}

// mark a block as available
static void mark_blk (int order, int blk_id)
{
    struct mark     *mk, *p;
    struct order    *ord;
    int             insert;
    
    ord = &kmem.orders[order - MIN_ORD];
    mk  = get_mark (order, blk_id >> 5);

    // whether we need to insert it into the list
    insert = (mk->bitmap == 0);

    // clear the bit map
    if (available(mk->bitmap, blk_id)) {
        panic ("double free\n");
    }
    
    mk->bitmap |= (1 << (blk_id & 0x1F));
    
    // just insert it to the head, no need to keep the list ordered
    if (insert) {
        blk_id >>= 5;
        mk->lnks = LNKS(NIL, ord->head);

        // fix the pre pointer of the next mark
        if (ord->head != NIL) {
            p = get_mark(order, ord->head);
            p->lnks = LNKS(blk_id, NEXT_LNK(p->lnks));
        }
        
        ord->head = blk_id;
    }
}

// get a block
static void* get_blk (int order)
{
    struct mark *mk;
    int blk_id;
    int i;
    struct order *ord;

    ord = &kmem.orders[order - MIN_ORD];
    mk = get_mark(order, ord->head);

    if (mk->bitmap == 0) {
        panic ("empty mark in the list\n");
    }

    for (i = 0; i < 32; i++) {
        if (mk->bitmap & (1 << i)) {
            blk_id = ord->head * 32 + i;
            unmark_blk(order, blk_id);
            
            return blkid2mem(order, blk_id);
        }
    }

    return NULL;
}

void _kfree (void *mem, int order);


static void *_kmalloc (int order)
{
    struct order *ord;
    uint8         *up;

    ord = &kmem.orders[order - MIN_ORD];
    up  = NULL;
    
    if (ord->head != NIL) {
        up = get_blk(order);
        
    } else if (order < MAX_ORD){
        // if currently no block available, try to split a parent
        up = _kmalloc (order + 1);

        if (up != NULL) {
            _kfree (up + (1 << order), order);
        }
    }

    return up;
}

// allocate memory that has the size of (1 << order)
void *kmalloc (int order)
{
    uint8         *up;

    if ((order > MAX_ORD) || (order < MIN_ORD)) {
        panic("kmalloc: order out of range\n");
    }

    acquire(&kmem.lock);
    up = _kmalloc(order);
    release(&kmem.lock);

    return up;
}

void _kfree (void *mem, int order)
{
    int blk_id, buddy_id;
    struct mark *mk;

    blk_id = mem2blkid(order, mem);
    mk = get_mark(order, blk_id >> 5);

    if (available(mk->bitmap, blk_id)) {
        panic ("kfree: double free");
    }

    buddy_id = blk_id ^ 0x0001; // blk_id and buddy_id differs in the last bit
                                // buddy must be in the same bit map
    if (!available(mk->bitmap, buddy_id) || (order == MAX_ORD)) {
        mark_blk(order, blk_id);
    } else {
        // our buddy is also free, merge it
        unmark_blk (order, buddy_id);
        _kfree (blkid2mem(order, blk_id & ~0x0001), order+1);
    }
}

// free kernel memory, we require order parameter here to avoid
// storing size info somewhere which might break the alignment
void kfree (void *mem, int order)
{
    if ((order > MAX_ORD) || (order < MIN_ORD) || (uint64)mem & ((1<<order) -1)) {
        panic("kfree: order out of range or memory unaligned\n");
    }

    acquire(&kmem.lock);
    _kfree(mem, order);
    release(&kmem.lock);
}

// free a page
void free_page(void *v)
{
    kfree (v, PTE_SHIFT);
}

// allocate a page
void* alloc_page (void)
{
    return kmalloc (PTE_SHIFT);
}

// round up power of 2, then get the order
//   http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
int get_order (uint32 v)
{
    uint32 ord;
    
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;

    for (ord = 0; ord < 32; ord++) {
        if (v & (1 << ord)) {
            break;
        }
    }
    
    if (ord < MIN_ORD) {
        ord = MIN_ORD;
    } else if (ord > MAX_ORD) {
        panic ("order too big!");
    }
    
    return ord;

}

