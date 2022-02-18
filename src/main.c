#include <kprint.h>
#include <common.h>
#include <sbi.h>
#include <utils.h>
#include <tinyshell.h>
#include <page.h>


int main(void){
    kprint_set_putc(sbi_putchar);
    kprint_set_getc(sbi_getchar);
    init_cont_page();
    page* tmp = page_cont_falloc(3);
    page_cont_free(tmp, 3);
    page* tmp2 = page_cont_falloc(3);
    tsh();
    return 0;
}
