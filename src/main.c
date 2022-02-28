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
    page_table* p_table = page_cont_falloc(1);
    kernel_page_table = p_table;
    mmu_init();
    imalloc_init();
    u64* i = imalloc(312);
    u64* i2 = imalloc(312);
    u64* i3 = imalloc(4000);
    u64* i4 = imalloc(8000);
    u64* i5 = imalloc(12000);
    imfree(i5);
    imfree(i4);
    print_flist();
    tsh();
    return 0;
}
