#include <kprint.h>
#include <common.h>
#include <sbi.h>
#include <utils.h>
#include <tinyshell.h>
#include <page.h>


int main(void){
    kprint_set_putc(sbi_putchar);
    kprint_set_getc(sbi_getchar);
    initialize_page();
    tsh();
    return 0;
}
