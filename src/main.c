#include <kprint.h>

int main(u64 hart){

    kprint_set_putc(sbi_putchar);
    kprint_set_getc(sbi_getchar);
    kprint("got to the OS from hart %U!\n", hart);
    return 0;
}
