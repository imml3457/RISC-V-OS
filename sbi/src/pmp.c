#include <pmp.h>

void pmp_init(){
    CSR_WRITE("pmpaddr0", 0xffffffff >> 2);
    CSR_WRITE("pmpcfg0", (0b01 << 3) | (1 << 2) | (1 << 1) | (1 << 0));
}
