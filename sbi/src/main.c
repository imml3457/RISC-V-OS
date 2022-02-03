#include <uart.h>
#include <kprint.h>
#include <plic.h>

int main(int hart){
    while(hart != 0);

    uart_init();

    plic_init();

    kprint_set_putc(uart_put);
    kprint_set_getc(uart_get);

    return 0;
}
