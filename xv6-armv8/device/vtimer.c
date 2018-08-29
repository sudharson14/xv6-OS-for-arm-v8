// AArch64  vitrual timer support
#include "types.h"
#include "param.h"
#include "arm.h"
#include "mmu.h"
#include "defs.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

#define CNTV_CTL_ENABLE  (1 << 0)
#define CNTV_CTL_MASK    (1 << 1)
#define CNTV_CTL_START   (1 << 2)

#define HZ_PER_US (1000000UL)
#define TIMER_INTERVAL_US (10000)

void isr_timer (struct trapframe *tp, int irq_idx);

struct spinlock tickslock;
uint ticks;

// Read physical counter
static inline uint64
aarch64_get_physical_counter(void) {
    uint64 val;
    
    __asm__ __volatile__("mrs %0, cntpct_el0\n\t" : "=r"(val));
    
    return val;
}

// Get the frequency of generic timers(UNIT:Hz)
static inline uint64
aarch64_get_generic_timer_freq(void) {
    uint64 val;
    
    __asm__ __volatile__("mrs %0, cntfrq_el0\n\t" : "=r"(val));
    
    return val;
}

// read control register
uint64
clock_read_ctrl(void) {
    uint64 cntv_ctrl;
    
    asm volatile ("isb; mrs %0, cntv_ctl_el0; isb;" 
	: "=r"(cntv_ctrl) :: "memory");
    
    return cntv_ctrl;
}

// write control register
void
clock_write_ctrl(uint64 cntv_ctrl) {

    asm volatile ("isb; msr cntv_ctl_el0, %0; isb;" 
	:: "r"(cntv_ctrl) : "memory");
    return ;
}

//write timer compare value
void
clock_write_tval(uint64 cntv_tval){

    asm volatile ("isb; msr cntv_tval_el0, %0; isb;" :: "r"(cntv_tval)
	: "memory");
}

void
stop_timer(void) {
    uint64 cntv_ctl;
    
    cntv_ctl = clock_read_ctrl();
    cntv_ctl &= ~( CNTV_CTL_ENABLE | CNTV_CTL_START ); //disable
    cntv_ctl |=  CNTV_CTL_MASK; //mask
    clock_write_ctrl(cntv_ctl);
}

void
start_timer(void) {
    uint64 cntv_ctl;
    
    cntv_ctl = clock_read_ctrl();
    cntv_ctl |=  ( CNTV_CTL_ENABLE | CNTV_CTL_START ); //enable
    cntv_ctl &= ~CNTV_CTL_MASK; //mask
    clock_write_ctrl(cntv_ctl);
}

// acknowledge the timer and reload value, write any value to cntvval_el0 should do
void
reload_timer(uint64 next_us){
    uint64 diff;
    
    diff = next_us * ( aarch64_get_generic_timer_freq() / HZ_PER_US  );
    clock_write_tval(diff);
}

// initialize the timer: perodical and interrupt based
void timer_init(int hz)
{
    stop_timer();
    reload_timer(TIMER_INTERVAL_US);
    start_timer();
    pic_enable (PIC_VTIMER, isr_timer);
}

// interrupt service routine for the timer
void isr_timer (struct trapframe *tp, int irq_idx)
{
    acquire(&tickslock);
    ticks++;
    wakeup(&ticks);
    release(&tickslock);
    stop_timer();
    reload_timer(TIMER_INTERVAL_US);
    start_timer();
}

// a short delay, use timer 1 as the source
void micro_delay (int us)
{
	uint64 v1, v2;
	uint64 diff;

	diff = us * ( aarch64_get_generic_timer_freq() / HZ_PER_US  );
	for(v1 = v2 = aarch64_get_physical_counter();
	    v2 < ( diff + v1 );
	    v2 = aarch64_get_physical_counter());
}
