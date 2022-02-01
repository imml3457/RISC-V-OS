#include <uart.h>

int main(int hart){
    while(hart != 0);

    uart_init();
    uart_write("hello world\n\n");


    while(1){
        char c;
        if ((c = uart_get()) != 0xff){
            uart_write("I got a character: ");
            uart_put(c);
            uart_put('\n');
        }
    }
    return 0;
}
