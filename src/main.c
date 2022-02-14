#include <kprint.h>
#include <common.h>
#include <sbi.h>
#include <utilities.h>
#include <tinyshell.h>


int main(void){
    kprint_set_putc(sbi_putchar);
    kprint_set_getc(sbi_getchar);
    tsh();
    return 0;
}
