#include <clint.h>
#include <kprint.h>
#include <csr.h>


void clint_set_msip(u64 hart){
    if (hart > 9){
        return;
    }
    volatile unsigned int* clint = (unsigned int*)CLINT_BASE_ADDRESS;
    clint[hart] = 1;
    //setting clint base address offset with the hart
    //to 1 to enable
}
void clint_clear_msip(u64 hart){
    if (hart > 9){
        return;
    }
    volatile unsigned int* clint = (unsigned int*)CLINT_BASE_ADDRESS;
    clint[hart] = 0;
    //like above but disable
}
void clint_set_mtimecmp(u64 hart, u64 timeval){
    if (hart > 9){
        return;
    }
    u64* clint_mtime = (u64*)(CLINT_BASE_ADDRESS + CLINT_MTIMECMP_OFFSET);
    clint_mtime[hart] = timeval;
}
u64 clint_get_time(void){
    u64 time;
    asm volatile ("rdtime %0" : "=r"(time));
    return time;
}
void clint_add_mtimecmp(u64 hart, u64 timeval){
    clint_set_mtimecmp(hart, clint_get_time() + timeval);
}

void handle_mtip(u64 cause, u64 hart){
    unsigned long sip;
    CSR_READ(sip, "mip");
    CSR_WRITE("mip", SET_SIP_STIP);

    kprint("TIMER\n");
    clint_set_mtimecmp(hart, CLINT_MTIMECMP_INFINITE);

}
