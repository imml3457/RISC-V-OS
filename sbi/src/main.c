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

    pmp_init();

    kprint_set_putc(uart_put);
    kprint_set_getc(uart_get);
    CSR_WRITE("mscratch", &SBI_GPREGS[hart].gpregs[0]);

    CSR_WRITE("sscratch", hart);

    CSR_WRITE("mepc", 0x80050000UL);
    CSR_WRITE("mideleg", (1 << 1) | (1 << 5) | (1 << 7));
    CSR_WRITE("medeleg", 0xB1FF);
    CSR_WRITE("mstatus", (1 << 13) | (1 << 11) | (1 << 7));

    asm volatile ("mret");
    return 0;
}
