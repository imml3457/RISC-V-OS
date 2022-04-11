#include <trap.h>
#include <kprint.h>
#include <plic.h>
#include <supcall.h>
#include <clint.h>
#include <hart.h>

void unhandled_irq(u64 cause, u64 hartid){
/*     kprint("get gud I haven't handled this yed %U on hart %U\n", cause, hartid); */
}

void (*irq_table[])(u64, u64) = {
    //sync
    unhandled_irq,
    unhandled_irq,
    unhandled_irq,
    unhandled_irq,
    unhandled_irq,
    unhandled_irq,
    unhandled_irq,
    unhandled_irq,
    unhandled_irq,
    supcall_handle,
    unhandled_irq,
    unhandled_irq,
    unhandled_irq,
    unhandled_irq,
    unhandled_irq,
    unhandled_irq,
    // async
    unhandled_irq,        //0
    unhandled_irq,
    unhandled_irq,
    h_msip,
    unhandled_irq,        //4
    unhandled_irq,
    unhandled_irq,
    clint_set_mtimecmp,
    unhandled_irq,
    unhandled_irq,        //9
    unhandled_irq,
    plic_handle_irq,        //plic_irq
};



void handle_irq(u64 cause, u64 hartid){
    irq_table[cause](cause, hartid);
}


void c_trap_handler(void){

    //getting the cause of the trap
    u64 mcause;
    u64 mhartid;
    CSR_READ(mcause, "mcause");
    CSR_READ(mhartid, "mhartid");

    u32 async_flag = MCAUSE_IS_ASYNC(mcause);
    mcause = MCAUSE_NUM(mcause);

    //use a table to determine the cause of the interrupt
    //i guess you can use a switch, but that shit is ugly

    if (async_flag){
        handle_irq(mcause + 16, mhartid);
    }

    else{
        handle_irq(mcause, mhartid);
    }

}
