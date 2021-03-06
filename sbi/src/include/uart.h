#ifndef __UART_H__
#define __UART_H__

#include <common.h>

enum MMIO_UART {
    UART_BASE = 0x10000000,
    UART_TXRX = 0,
    UART_IER = 1,
    UART_FCR = 2,
    UART_LCR = 3,
    UART_LSR = 5,
};


void uart_init(void);

void uart_write(const char *s);

void uart_put(u8 c);

u8 uart_get(void);

u8 uart_get_char(void);

void uart_handle_irq(void);

#endif
