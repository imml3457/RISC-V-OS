#include <uart.h>
#include <kprint.h>

// UART BASE 0x1000_0000
//1 byte registers


void uart_init(void){
    volatile unsigned char *uart = (unsigned char *)UART_BASE;

    uart[UART_LCR] = (1 << 0) | (1 << 1);

    uart[UART_FCR] = 1;

    uart[UART_IER] = 1;

}

void uart_write(const char *s){
    while (*s){
        uart_put(*s);
        s++;
    }
}

void uart_put(u8 c){
    volatile unsigned char *uart = (unsigned char *)UART_BASE;
    //check to see if trasmitter is empty
    //if so send it
    if(uart[UART_LSR] & (1 << 6)){
        uart[UART_TXRX] = c;
    }

}

u8 uart_get(void){
    volatile unsigned char *uart = (unsigned char *)UART_BASE;

    if(!(uart[UART_LSR] & 1)) {
        //if no data is ready return 255
        return 0xff;
    }
    else{
        //if data is ready send the reciever buffer register
        //this actually holds the character (in 8 bits)
        return uart[UART_TXRX];
    }
}


void uart_handle_irq(void){
    char c = uart_get();
    kprint("uart got %d (%d)\n", c, c);
    //hey this should be ringbuffer

}
