#include <uart.h>
#include <kprint.h>
#include <plic.h>
#include <pmp.h>
#include <trap.h>
#include <asm_common.h>
struct trapframe SBI_GPREGS[8];

ATTR_NAKED_NORET
void main(int hart){
    while(hart != 0){
        asm volatile ("wfi");
    }

    clear_bss();
    uart_init();


    plic_init();

    pmp_init();

    kprint_set_putc(uart_put);
    kprint_set_getc(uart_get);
    CSR_WRITE("mscratch", &SBI_GPREGS[hart].gpregs[0]);
    CSR_WRITE("sscratch", hart);
    CSR_WRITE("mepc", OS_LOAD_ADDR);
    CSR_WRITE("mtvec", sbi_trap_vector);
    CSR_WRITE("mie", SET_MIE_MEIE | SET_MIE_MTIE | SET_MIE_MSIE);
    CSR_WRITE("mideleg", SET_MIP_SEIP | SET_MIP_STIP | SET_MIP_MTIP);
    CSR_WRITE("medeleg", MEDELEG_ALL);
    CSR_WRITE("mstatus", MSTATUS_FS_INITIAL | MSTATUS_SET_SUPERVISOR | MSTATUS_SET_MPIE);
    asm volatile ("mret");
}
