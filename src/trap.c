#include <trap.h>
#include <kprint.h>
#include <plic.h>
#include <sbi.h>

void unhandled_irq(u64 cause, u32 hartid){
/*     kprint("Unhandled IRQ cause #: %U on Hart: %u\n", cause, hartid); */
/*     sbi_system_off(); */
}

void (*irq_table[])(u64, u32) = {
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
    unhandled_irq,
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
    unhandled_irq,
    unhandled_irq,        //4
    unhandled_irq,
    unhandled_irq,
    unhandled_irq,
    unhandled_irq,
    plic_handle_irq,        //plic_irq
    unhandled_irq,        //9
    unhandled_irq,
};



void handle_irq(u64 cause, u32 hartid){
    irq_table[cause](cause, hartid);
}


void sup_trap_handler(void){

    //getting the cause of the trap
    u64 scause;
    CSR_READ(scause, "scause");

    u32 hart = sbi_whoami();

    u32 async_flag = SCAUSE_IS_ASYNC(scause);
    scause = SCAUSE_NUM(scause);

    //use a table to determine the cause of the interrupt
    //i guess you can use a switch, but that shit is ugly

    if (async_flag){
        handle_irq(scause + 16, hart);
    }

    else{
        handle_irq(scause, hart);
    }

}
