#include <kprint.h>
#include <common.h>
#include <sbi.h>
#include <utils.h>
#include <tinyshell.h>
#include <page.h>
#include <imalloc.h>
#include <mmu.h>
#include <symbols.h>


int main(void){
    kprint_set_putc(sbi_putchar);
    kprint_set_getc(sbi_getchar);
    init_cont_page();
    mmu_init();
    imalloc_init();
    tsh();
    return 0;
}
