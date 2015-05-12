// Definition for ARM MMU
#ifndef MMU_INCLUDE
#define MMU_INCLUDE

// align_up/down: al must be of power of 2
#define align_up(sz, al) (((uint64)(sz)+ (uint64)(al)-1) & ~((uint64)(al)-1))
#define align_dn(sz, al) ((uint64)(sz) & ~((uint64)(al)-1))
//
// Since ARMv6, you may use two page tables, one for kernel pages (TTBR1),
// and one for user pages (TTBR0). We use this architecture. Memory address
// lower than UVIR_BITS^2 is translated by TTBR0, while higher memory is
// translated by TTBR1.
// Kernel pages are create statically during system initialization. It use
// 2MB page mapping. User pages use 4K pages.
//


// access permission for page directory/page table entries.
//#define AP_NA       0x00    // no access
//#define AP_KO       0x01    // privilaged access, kernel: RW, user: no access
//#define AP_KUR      0x02    // no write access from user, read allowed
//#define AP_KU       0x03    // full access

// domain definition for page table entries
//#define DM_NA       0x00    // any access causing a domain fault
//#define DM_CLIENT   0x01    // any access checked against TLB (page table)
//#define DM_RESRVED  0x02    // reserved
//#define DM_MANAGER  0x03    // no access check

//#define PE_CACHE    (1 << 3)// cachable
//#define PE_BUF      (1 << 2)// bufferable

//#define PE_TYPES    0x03    // mask for page type
//#define KPDE_TYPE   0x02    // use "section" type for kernel page directory
//#define UPDE_TYPE   0x01    // use "coarse page table" for user page directory
//#define PTE_TYPE    0x02    // executable user page(subpage disable)

#define ENTRY_VALID	0x01
#define ENTRY_BLOCK	0x00
#define ENTRY_TABLE	0x02
#define ENTRY_PAGE	0x02
#define ENTRY_MASK	0x03
#define ENTRY_FALUT	0x00

#define MEM_ATTR_IDX_0	(0 << 2)
#define MEM_ATTR_IDX_1	(1 << 2)
#define MEM_ATTR_IDX_2	(2 << 2)
#define MEM_ATTR_IDX_3	(3 << 2)
#define MEM_ATTR_IDX_4	(4 << 2)
#define MEM_ATTR_IDX_5	(5 << 2)
#define MEM_ATTR_IDX_6	(6 << 2)
#define MEM_ATTR_IDX_7	(7 << 2)

#define NON_SECURE_PA (1 << 5)

#define AP_RW_1     (0 << 6)
#define AP_RW_1_0   (1 << 6)
#define AP_RO_1     (2 << 6)
#define AP_RO_1_0   (3 << 6)
#define AP_MASK     (3 << 6)

#define SH_NON_SH   (0 << 8)
#define SH_UNPRED   (1 << 8)
#define SH_OUT_SH   (2 << 8)
#define SH_IN_SH    (3 << 8)

#define ACCESS_FLAG (1 << 10)

#define PXN         (0x20000000000000)
#define UXN         (0x40000000000000)


#define PG_ADDR_MASK	0xFFFFFFFFF000	// bit 47 - bit 12

// 1st level 
#define PGD_SHIFT	30
#define PGD_SZ		(1 << PGD_SHIFT)
#define PGD_MASK 	(~(PGD_SZ - 1))			// offset for page directory entries
#define PTRS_PER_PGD	4
#define PGD_IDX(v)	(((uint64)(v) >> PGD_SHIFT) & (PTRS_PER_PGD - 1))	// index for page table entry

// 2nd-level (2MB) page directory (always maps 1MB memory)
#define PMD_SHIFT	21					// shift how many bits to get PDE index
#define PMD_SZ      	(1 << PMD_SHIFT)
#define PMD_MASK	(~(PMD_SZ - 1))				// offset for page directory entries
#define PTRS_PER_PMD	512
#define PMD_IDX(v)	(((uint64)(v) >> PMD_SHIFT) & (PTRS_PER_PMD - 1))	// index for page table entry

// 3rd-level (4k) page table
#define PTE_SHIFT	12					// shift how many bits to get PTE index
#define PTE_SZ		(1 << PTE_SHIFT)
#define PTRS_PER_PTE	512
#define PTE_ADDR(v)	align_dn (v, PTE_SZ)
#define PTE_IDX(v)	(((uint64)(v) >> PTE_SHIFT) & (PTRS_PER_PTE - 1))
#define PTE_AP(pte)	(pte & AP_MASK)

// size of two-level page tables
#define UADDR_BITS	28					// maximum user-application memory, 256MB
#define UADDR_SZ	(1 << UADDR_BITS)			// maximum user address space size

// must have NUM_UPDE == NUM_PTE
//#define NUM_UPDE	(1 << (UADDR_BITS - PMD_SHIFT))		// # of PDE for user space
//#define NUM_PTE	(1 << (PMD_SHIFT - PTE_SHIFT))		// how many PTE in a PT

#define PT_SZ		(PTRS_PER_PTE << 3)			// user page table size (1K)
#define PT_ADDR(v)	align_dn(v, PT_SZ)			// physical address of the PT
#define PT_ORDER	10

#endif
