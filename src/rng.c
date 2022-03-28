#include <rng.h>
#include <rng_driver.h>



void rng(void* buffer, u16 size){
    virt_rng_drive(buffer, size);
}
