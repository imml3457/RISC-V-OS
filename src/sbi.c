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
