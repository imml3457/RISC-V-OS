#include <trap.h>
#include <kprint.h>
#include <plic.h>

void c_trap_handler(void){

    //getting the cause of the trap
    s64 mcause;
    s64 mhartid;
    CSR_READ(mcause, "mcause");
    CSR_READ(mhartid, "mhartid");

    u32 async_flag = (mcause >> 63) & 1;
    mcause &= 0xff;

    //use a table to determine the cause of the interrupt
    //i guess you can use a switch, but that shit is ugly

    if (async_flag){
        switch (mcause) {
            case 11:
                plic_handle_irq(mhartid);
                break;
            default:
                kprint("unhandled async interrupt %I\n", mcause);
                break;
        }

    }

}
