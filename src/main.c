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

    mmu_map(p_table, sym_start(text), sym_start(text), sym_size(text), re);
    mmu_map(p_table, sym_start(data), sym_start(data), sym_size(data), rw);
    mmu_map(p_table, sym_start(rodata), sym_start(rodata), sym_size(rodata), read);
    mmu_map(p_table, sym_start(bss), sym_start(bss), sym_size(bss), rw);
    mmu_map(p_table, sym_start(stack), sym_start(stack), sym_size(stack), rw);
    mmu_map(p_table, sym_start(heap), sym_start(heap), sym_size(heap), rw);



    mmu_map(p_table, 0x0c000000, 0x0c000000, 0x0c2fffff - 0x0c000000, rw);
    CSR_WRITE("satp", SV39 | SET_ASID(OS_ASID) | SET_PPN(p_table));
    sfence();


    imalloc_init();
    kprint("I got past sfence\n");
    tsh();
    return 0;
}
