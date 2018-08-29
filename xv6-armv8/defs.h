//
// a definition of data structure/methods
//
#ifndef INCLUDE_DEFS_H
#define INCLUDE_DEFS_H

#define UMAX(a, b) ((uint64)(a) > (uint64)(b) ? (uint64)(a):(uint64)(b))
#define UMIN(a, b) ((uint64)(a) > (uint64)(b) ? (uint64)(b):(uint64)(a))

// number of elements in fixed-size array
#define NELEM(x) (sizeof(x)/sizeof((x)[0]))

struct buf;
struct context;
struct file;
struct inode;
struct pipe;
struct proc;
struct spinlock;
struct stat;
struct superblock;
struct trapframe;

typedef uint64	pte_t;
typedef uint64  pmd_t;
typedef uint64  pgd_t;
extern  uint64  _kernel_pgtbl;
typedef void (*ISR) (struct trapframe *tf, int n);

// arm.c
void            set_stk(uint mode, uint addr);
void            cli (void);
void            sti (void);
int             int_enabled();
void            pushcli(void);
void            popcli(void);
void            getcallerpcs(void *, uint64*);
void*           get_fp (void);
void            show_callstk (char *);


// bio.c
void            binit(void);
struct buf*     bread(uint, uint);
void            brelse(struct buf*);
void            bwrite(struct buf*);

// buddy.c
void            kmem_init (void);
void            kmem_init2(void *vstart, void *vend);
void*           kmalloc (int order);
void            kfree (void *mem, int order);
void            free_page(void *v);
void*           alloc_page (void);
void            kmem_test_b (void);
int             get_order (uint32 v);

// console.c
void            consoleinit(void);
void            cprintf(char*, ...);
void		cprintf2 (char *fmt, uint64 arg, ...);
void            consoleintr(int(*)(void));
void            panic(char*) __attribute__((noreturn));

// exec.c
int             exec(char*, char**);

// file.c
struct file*    filealloc(void);
void            fileclose(struct file*);
struct file*    filedup(struct file*);
void            fileinit(void);
int             fileread(struct file*, char*, int n);
int             filestat(struct file*, struct stat*);
int             filewrite(struct file*, char*, int n);

// fs.c
void            readsb(int dev, struct superblock *sb);
int             dirlink(struct inode*, char*, uint);
struct inode*   dirlookup(struct inode*, char*, uint*);
struct inode*   ialloc(uint, short);
struct inode*   idup(struct inode*);
void            iinit(void);
void            ilock(struct inode*);
void            iput(struct inode*);
void            iunlock(struct inode*);
void            iunlockput(struct inode*);
void            iupdate(struct inode*);
int             namecmp(const char*, const char*);
struct inode*   namei(char*);
struct inode*   nameiparent(char*, char*);
int             readi(struct inode*, char*, uint, uint);
void            stati(struct inode*, struct stat*);
int             writei(struct inode*, char*, uint, uint);

// ide.c
void            ideinit(void);
void            iderw(struct buf*);

// kalloc.c
/*char*           kalloc(void);
void            kfree(char*);
void            kinit1(void*, void*);
void            kinit2(void*, void*);
void            kmem_init (void);*/

// log.c
void            initlog(void);
void            log_write(struct buf*);
void            begin_trans();
void            commit_trans();

// picirq.c
void            pic_enable(int, ISR);
void            pic_init(void*);
int             pic_dispatch (struct trapframe *tp);

// pipe.c
int             pipealloc(struct file**, struct file**);
void            pipeclose(struct pipe*, int);
int             piperead(struct pipe*, char*, int);
int             pipewrite(struct pipe*, char*, int);

//PAGEBREAK: 16
// proc.c
struct proc*    copyproc(struct proc*);
void            exit(void);
int             fork(void);
int             growproc(int);
int             kill(int);
void            pinit(void);
void            procdump(void);
void            scheduler(void) __attribute__((noreturn));
void            sched(void);
void            sleep(void*, struct spinlock*);
void            userinit(void);
int             wait(void);
void            wakeup(void*);
void            yield(void);

// swtch.S
void            swtch(struct context**, struct context*);

// spinlock.c
void            acquire(struct spinlock*);
int             holding(struct spinlock*);
void            initlock(struct spinlock*, char*);
void            release(struct spinlock*);

// string.c
int             memcmp(const void*, const void*, uint);
void*           memmove(void*, const void*, uint);
void*           memset(void*, int, uint);
char*           safestrcpy(char*, const char*, int);
int             strlen(const char*);
int             strncmp(const char*, const char*, uint);
char*           strncpy(char*, const char*, int);

// syscall.c
int             argint(int, long*);
int             argptr(int, char**, int);
int             argstr(int, char**);
int             fetchint(uint64, long*);
int             fetchstr(uint64, char**);
void            syscall(void);

// timer.c
void            timer_init(int hz);
extern struct   spinlock tickslock;

// trap.c
extern uint     ticks;
void            trap_init(void);
void            dump_trapframe (struct trapframe *tf);

// trap_asm.S
void            trap_reset(void);

// uart.c
void            uart_init(void*);
void            uartputc(int);
int             uartgetc(void);
void            micro_delay(int us);
void            uart_enable_rx();

// vm.c
int             allocuvm(pgd_t*, uint, uint);
int             deallocuvm(pgd_t*, uint, uint);
void            freevm(pgd_t*);
void            inituvm(pgd_t*, char*, uint);
int             loaduvm(pgd_t*, char*, struct inode*, uint, uint);
pmd_t*          copyuvm(pgd_t*, uint);
void            switchuvm(struct proc*);
int             copyout(pgd_t*, uint, void*, uint);
void            clearpteu(pgd_t *pgdir, char *uva);
void*           kpt_alloc(void);
void            init_vmm (void);
void            kpt_freerange (uint64 low, uint64 hi);
void            paging_init (uint64 phy_low, uint64 phy_hi);

// gic.c
void 		gic_init(void* base);

#endif
