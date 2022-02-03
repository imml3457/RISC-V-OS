#include <trap.h>


struct trapframe c_sbi_trap_frame;

void c_trap_handler(void){

    //getting the cause of the trap
    s64 mcause;
    CSR_READ(mcause, "mcause");

    //use a table to determine the cause of the interrupt
    //i guess you can use a switch, but that shit is ugly


}
