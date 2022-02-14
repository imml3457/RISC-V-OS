#include <clint.h>
#include <kprint.h>


void clint_set_msip(u64 hart){
    if (hart >= 8){
        return;
    }
    volatile unsigned int* clint = (unsigned int*)CLINT_BASE_ADDRESS;
    clint[hart] = 1;
    //setting clint base address offset with the hart
    //to 1 to enable
}
void clint_clear_msip(u64 hart){
    if (hart >= 8){
        return;
    }
    kprint("clearing msip\n");
    volatile unsigned int* clint = (unsigned int*)CLINT_BASE_ADDRESS;
    clint[hart] = 0;
    //like above but disable
}
void clint_set_mtimecmp(u64 hart, u64 timeval){
    if (hart > 9){
        return;
    }
    (void) timeval;
    u64* clint_mtime = (u64*)(CLINT_BASE_ADDRESS + CLINT_MTIMECMP_OFFSET);
    clint_mtime[hart] = CLINT_MTIMECMP_INFINITE;
}
u64 clint_get_time(void){
    u64 time;
    asm volatile ("rdtime %0" : "=r"(time));
    return time;
}
