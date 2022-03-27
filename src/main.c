#include <kprint.h>
#include <common.h>
#include <sbi.h>
#include <utils.h>
#include <tinyshell.h>
#include <page.h>
#include <imalloc.h>
#include <mmu.h>
#include <symbols.h>
#include <pci.h>
#include <plic.h>
#include <trap.h>

struct trapframe SUP_GPREGS[8];

void sup_trap_vector(void);

int main(void){
    kprint_set_putc(sbi_putchar);
    kprint_set_getc(sbi_getchar);
/*     plic_init(); */
    CSR_WRITE("stvec", sup_trap_vector);
    CSR_WRITE("sscratch", &(SUP_GPREGS[0].gpregs[0]));
    CSR_WRITE("sie", (1 << 9));
    init_cont_page();
    page_table* p_table = page_cont_falloc(1);
    kernel_page_table = p_table;
    mmu_init();
    imalloc_init();
    initpci();
    pci_set_capes();
    struct PCIdriver* driver = find_driver(VIRTIO_VENDOR, RNG_DEVICE);
/*     char bytes[5] = {0}; */
/*     driver->drive_rng(bytes, sizeof(bytes)); */
/*     WFI(); */
/*     kprint("%02x %02x %02x %02x %02x\n", bytes[0], bytes[1], bytes[2], bytes[3], bytes[4]); */
    tsh();
    return 0;
}
