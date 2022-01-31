#ifndef __UART_H__
#define __UART_H__



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

void uart_put(char c);

char uart_get(void);

#endif
