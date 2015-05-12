#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "arm.h"
#include "syscall.h"

// User code makes a system call with INT T_SYSCALL. System call number
// in r0. Arguments on the stack, from the user call to the C library
// system call function. The saved user sp points to the first argument.

// Fetch the int at addr from the current process.
int fetchint(uint64 addr, long *ip)
{
    if(addr >= proc->sz || addr+8 > proc->sz) {
        return -1;
    }

    *ip = *(long*)(addr);
    return 0;
}

// Fetch the nul-terminated string at addr from the current process.
// Doesn't actually copy the string - just sets *pp to point at it.
// Returns length of string, not including nul.
int fetchstr(uint64 addr, char **pp)
{
    char *s, *ep;

    if(addr >= proc->sz) {
        return -1;
    }

    *pp = (char*)addr;
    ep = (char*)proc->sz;

    for(s = *pp; s < ep; s++) {
        if(*s == 0) {
            return s - *pp;
        }
    }

    return -1;
}

// Fetch the nth (starting from 0) 32-bit system call argument.
// In our ABI, r0 contains system call index, r1-r4 contain parameters.
// now we support system calls with at most 4 parameters.
int argint(int n, long *ip)
{
    if (n > 3) {
        panic ("too many system call parameters\n");
    }

    *ip = *(&proc->tf->r1 + n);

    return 0;
}

// Fetch the nth word-sized system call argument as a pointer
// to a block of memory of size n bytes.  Check that the pointer
// lies within the process address space.
int argptr(int n, char **pp, int size)
{
    long i;

    if(argint(n, &i) < 0) {
        return -1;
    }

    if((uint64)i >= proc->sz || (uint64)i+size > proc->sz) {
        return -1;
    }

    *pp = (char*)i;
    return 0;
}

// Fetch the nth word-sized system call argument as a string pointer.
// Check that the pointer is valid and the string is nul-terminated.
// (There is no shared writable memory, so the string can't change
// between this check and being used by the kernel.)
int argstr(int n, char **pp)
{
    long addr;

    if(argint(n, &addr) < 0) {
        return -1;
    }

    return fetchstr(addr, pp);
}

extern int sys_chdir(void);
extern int sys_close(void);
extern int sys_dup(void);
extern int sys_exec(void);
extern int sys_exit(void);
extern int sys_fork(void);
extern int sys_fstat(void);
extern int sys_getpid(void);
extern int sys_kill(void);
extern int sys_link(void);
extern int sys_mkdir(void);
extern int sys_mknod(void);
extern int sys_open(void);
extern int sys_pipe(void);
extern int sys_read(void);
extern int sys_sbrk(void);
extern int sys_sleep(void);
extern int sys_unlink(void);
extern int sys_wait(void);
extern int sys_write(void);
extern int sys_uptime(void);

static int (*syscalls[])(void) = {
        [SYS_fork]    sys_fork,
        [SYS_exit]    sys_exit,
        [SYS_wait]    sys_wait,
        [SYS_pipe]    sys_pipe,
        [SYS_read]    sys_read,
        [SYS_kill]    sys_kill,
        [SYS_exec]    sys_exec,
        [SYS_fstat]   sys_fstat,
        [SYS_chdir]   sys_chdir,
        [SYS_dup]     sys_dup,
        [SYS_getpid]  sys_getpid,
        [SYS_sbrk]    sys_sbrk,
        [SYS_sleep]   sys_sleep,
        [SYS_uptime]  sys_uptime,
        [SYS_open]    sys_open,
        [SYS_write]   sys_write,
        [SYS_mknod]   sys_mknod,
        [SYS_unlink]  sys_unlink,
        [SYS_link]    sys_link,
        [SYS_mkdir]   sys_mkdir,
        [SYS_close]   sys_close,
};

void syscall(void)
{
    int num;
    int ret;

    num = proc->tf->r0;

    //cprintf ("syscall(%d) from %s(%d)\n", num, proc->name, proc->pid);

    if((num > 0) && (num <= NELEM(syscalls)) && syscalls[num]) {
        ret = syscalls[num]();

        // in ARM, parameters to main (argc, argv) are passed in r0 and r1
        // do not set the return value if it is SYS_exec (the user program
        // anyway does not expect us to return anything).
        if (num != SYS_exec) {
            proc->tf->r0 = ret;
        }
    } else {
        cprintf("%d %s: unknown sys call %d\n", proc->pid, proc->name, num);
        proc->tf->r0 = -1;
    }
}
