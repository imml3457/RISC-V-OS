#include <sbi.h>


void sbi_putchar(u8 c){
    asm volatile ("mv a7, %0\nmv a0, %1\necall" :: "r"(SBI_PUTCHAR), "r"(c) : "a7", "a0");
}

u8 sbi_getchar(){
    char c;
    asm volatile ("mv a7, %1\necall\nmv %0, a0\n" : "=r"(c) : "r"(SBI_GETCHAR) : "a7", "a0");
    return c;
}

u64 sbi_hart_status(u64 hart){
    u64 ret;
    asm volatile ("mv a7, %1\nmv a0, %2\necall\nmv %0, a0\n" : "=r"(ret) : "r"(SBI_HART_STATUS), "r"(hart) : "a0", "a7");
    return ret;
}

void sbi_system_off(){
    asm volatile ("mv a7, %0\necall" : : "r"(SBI_SYSTEM_OFF) : "a0", "a7");
}

u64 sbi_start_hart(u64 hart, u64 target, u64 privilege){
    int stat;
    asm volatile ("mv a7, %1\nmv a0, %2\nmv a1, %3\nmv a2, %4\necall\nmv %0, a0\n" :
                  "=r"(stat) :
                  "r"(SBI_START_HART), "r"(hart), "r"(target), "r"(privilege) :
                  "a0", "a1", "a2", "a7");
    return stat;
}

u64 sbi_stop_hart(void) {
    u64 stat;
    asm volatile ("mv a7, %1\necall\nmv a0, %0" : "=r"(stat) :
                  "r"(SBI_STOP_HART) : "a0", "a7");
    return stat;
}

u64 sbi_get_time(void){
    u64 ret;
    asm volatile("mv a7, %1\necall\nmv %0, a0" : "=r"(ret) : "r"(SBI_GET_TIME) : "a0", "a7");
    return ret;
}

void sbi_set_timecmp(u64 hart, u64 value){
    asm volatile("mv a7, %0\nmv a0, %1\nmv a1, %2\necall" :: "r"(SBI_SET_TIMECMP), "r"(hart), "r"(value) : "a0", "a1", "a7");
}

u32 sbi_whoami(void){
    u32 ret;
    asm volatile("mv a7, %1\necall\nmv %0, a0" : "=r"(ret) : "r"(SBI_WHOAMI) : "a0", "a7");
    return ret;
}
