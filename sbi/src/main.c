#include <uart.h>
#include <kprint.h>
#include <plic.h>
#include <pmp.h>
#include <trap.h>

struct trapframe SBI_GPREGS[8];

int main(int hart){
    while(hart != 0){
        asm volatile ("wfi");
    }

    uart_init();

    plic_init();

/*     pmp_init(); */

    kprint_set_putc(uart_put);
    kprint_set_getc(uart_get);
    CSR_WRITE("mscratch", &SBI_GPREGS[hart].gpregs[0]);


/*     asm volatile ("mret"); */
    return 0;
}
