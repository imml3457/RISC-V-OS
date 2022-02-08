#include <kprint.h>
#include <common.h>
#include <sbi.h>

int main(u64 hart){

    kprint_set_putc(sbi_putchar);
    kprint_set_getc(sbi_getchar);
    kprint("got to the OS from hart %U!\n", hart);
    u8 c;
    while(1){
        while((c = sbi_getchar()) == 0xff){ asm volatile("wfi");}
        //handle return keycode
        if(c == '\r'){
            kprint("\n");
        }
        else{
            kprint("Character: %u\n", c);
        }
    }

    return 0;
}
