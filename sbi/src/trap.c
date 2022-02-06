#include <trap.h>
#include <kprint.h>

void c_trap_handler(void){

    //getting the cause of the trap
    s64 mcause;
    s64 mhartid;
    CSR_READ(mcause, "mcause");
    CSR_READ(mhartid, "mhartid");
    kprint("mcause = 0x%X, mhartid = %U\n", mcause, mhartid);

    while(1){}
    //use a table to determine the cause of the interrupt
    //i guess you can use a switch, but that shit is ugly


}
