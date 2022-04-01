#include <trap.h>
#include <kprint.h>
#include <plic.h>

void unhandled_irq(u64 cause){
/*     kprint("Unhandled IRQ cause #: %U\n", cause); */
}

void (*irq_table[])(u64) = {
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



void handle_irq(u64 cause){
    irq_table[cause](cause);
}


void sup_trap_handler(void){

    //getting the cause of the trap
    u64 scause;
    CSR_READ(scause, "scause");

    u32 async_flag = SCAUSE_IS_ASYNC(scause);
    scause = SCAUSE_NUM(scause);

    //use a table to determine the cause of the interrupt
    //i guess you can use a switch, but that shit is ugly

    if (async_flag){
        handle_irq(scause + 16);
    }

    else{
        handle_irq(scause);
    }

}
