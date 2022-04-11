#include <uart.h>
#include <kprint.h>
#include <plic.h>
#include <pmp.h>
#include <trap.h>
#include <asm_common.h>
#include <lock.h>
#include <hart.h>

struct trapframe SBI_GPREGS[8];

Mutex hart0_lock = 1;

ATTR_NAKED_NORET
void main(int hart){

    if(hart != 0){
        //we need to initiate pmp for every hart
        mutex_spinlock(&hart0_lock);
        mutex_unlock(&hart0_lock);
        pmp_init();

        if(hart < 8){
            sbi_hart_data[hart].status = H_STOPPED;
            sbi_hart_data[hart].mode = H_MACHINE;
            sbi_hart_data[hart].target_addr = 0;

            CSR_WRITE("mscratch", &SBI_GPREGS[hart].gpregs[0]);
            CSR_WRITE("sscratch", hart);
        }
        CSR_WRITE("mepc", park);
        CSR_WRITE("mtvec", sbi_trap_vector);
        CSR_WRITE("mie", SET_MIE_MSIE);
        CSR_WRITE("mideleg", 0);
        CSR_WRITE("medeleg", 0);
        CSR_WRITE("mstatus", MSTATUS_FS_INITIAL | MSTATUS_SET_MACHINE | MSTATUS_SET_MPIE);
        MRET();
    }

    clear_bss();
    uart_init();
    mutex_unlock(&hart0_lock);
    plic_init();
    pmp_init();
    sbi_hart_data[hart].status = H_STARTED;
    sbi_hart_data[hart].mode = H_MACHINE;
    sbi_hart_data[hart].target_addr = 0;

    kprint_set_putc(uart_put);
    kprint_set_getc(uart_get);
    CSR_WRITE("mscratch", &SBI_GPREGS[hart].gpregs[0]);
    CSR_WRITE("sscratch", hart);
    CSR_WRITE("mepc", OS_LOAD_ADDR);
    CSR_WRITE("mtvec", sbi_trap_vector);
    CSR_WRITE("mie", SET_MIE_MEIE | SET_MIE_MTIE | SET_MIE_MSIE);
    CSR_WRITE("mideleg", SET_MIP_SEIP | SET_MIP_STIP | SET_MIP_SSIP);
    CSR_WRITE("medeleg", MEDELEG_ALL);
    CSR_WRITE("mstatus", MSTATUS_FS_INITIAL | MSTATUS_SET_SUPERVISOR | MSTATUS_SET_MPIE);
    MRET();
}
