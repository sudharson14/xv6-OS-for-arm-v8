// driver for ARM PrimeCell UART (PL011)
#include "types.h"
#include "defs.h"
#include "param.h"
#include "arm.h"
#include "memlayout.h"

static volatile uint *uart_base;
void isr_uart (struct trapframe *tf, int idx);

#define UART_DR		0	// data register
#define UART_RSR	1	// receive status register/error clear register
#define UART_FR		6	// flag register
#define	UART_IBRD	9	// integer baud rate register
#define UART_FBRD	10	// Fractional baud rate register
#define UART_LCR	11	// line control register
#define UART_CR		12	// control register
#define UART_IMSC	14	// interrupt mask set/clear register
#define UART_MIS	16	// masked interrupt status register
#define	UART_ICR	17	// interrupt clear register
// bits in registers
#define UARTFR_TXFF	(1 << 5)	// tramit FIFO full
#define UARTFR_RXFE	(1 << 4)	// receive FIFO empty
#define	UARTCR_RXE	(1 << 9)	// enable receive
#define UARTCR_TXE	(1 << 8)	// enable transmit
#define	UARTCR_EN	(1 << 0)	// enable UART
#define UARTLCR_FEN	(1 << 4)	// enable FIFO
#define UART_RXI	(1 << 4)	// receive interrupt
#define UART_TXI	(1 << 5)	// transmit interrupt
#define UART_BITRATE 19200

// enable uart
void uart_init (void *addr)
{
    uint left;

    uart_base = addr;

    // set the bit rate: integer/fractional baud rate registers
    uart_base[UART_IBRD] = UART_CLK / (16 * UART_BITRATE);

    left = UART_CLK % (16 * UART_BITRATE);
    uart_base[UART_FBRD] = (left * 4 + UART_BITRATE / 2) / UART_BITRATE;

    // enable trasmit and receive
    uart_base[UART_CR] |= (UARTCR_EN | UARTCR_RXE | UARTCR_TXE);

    // enable FIFO
    uart_base[UART_LCR] |= UARTLCR_FEN;
}

// enable the receive (interrupt) for uart (after PIC has initialized)
void uart_enable_rx ()
{
    uart_base[UART_IMSC] = UART_RXI;
    pic_enable(PIC_UART0, isr_uart);
}

void uartputc (int c)
{
    // wait a short period if the transmit FIFO is full
    while (uart_base[UART_FR] & UARTFR_TXFF) {
        micro_delay(10);
    }

    uart_base[UART_DR] = c;
}

//poll the UART for data
int uartgetc (void)
{
    if (uart_base[UART_FR] & UARTFR_RXFE) {
        return -1;
    }

    return uart_base[UART_DR];
}

void isr_uart (struct trapframe *tf, int idx)
{
    if (uart_base[UART_MIS] & UART_RXI) {
        consoleintr(uartgetc);
    }

    // clear the interrupt
    uart_base[UART_ICR] = UART_RXI | UART_TXI;
}
