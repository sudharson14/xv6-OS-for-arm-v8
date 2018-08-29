//
// Board specific information for the ARM Virtual Machine
//
#ifndef __ARM_VIRT__
#define __ARM_VIRT__


// the VerstatilePB board can support up to 256MB memory.
// but we assume it has 128MB instead. During boot, the lower
// 64MB memory is mapped to the flash, needs to be remapped
// the the SDRAM. We skip this for QEMU
#define PHY_START       0x40000000
#define PHYSTOP         (0x08000000 + PHY_START)

#define DEVBASE1        0x08000000
#define DEVBASE2        0x09000000
#define DEVBASE3        0x0a000000
#define DEV_MEM_SZ      0x01000000


#define UART0           0x09000000
#define UART_CLK        24000000    // Clock rate for UART

#define TIMER0          0x1c110000
#define TIMER1          0x1c120000
#define CLK_HZ          1000000     // the clock is 1MHZ

#define VIC_BASE        0x08000000

#define GICD_INTNO_SGIO         (0)
#define GICD_INTNO_PPIO         (16)
#define GICD_INTNO_SPIO         (32)

#define PIC_TIMER01     ( GICD_INTNO_SPIO + 13 )
#define PIC_TIMER23     ( GICD_INTNO_SPIO + 11 )
#define PIC_UART0       ( GICD_INTNO_SPIO + 1  )
#define PIC_GRAPHIC     ( GICD_INTNO_SPIO + 19 )
#define PIC_VTIMER      ( GICD_INTNO_PPIO + 11 )
#endif /* __ARM_VIRT__ */
