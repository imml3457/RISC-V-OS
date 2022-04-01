#include <block.h>
#include <block_driver.h>

void dsk_read(void* buffer, u64 addr_block, u64 size){
    virt_block_drive_read(buffer, addr_block, size);
}
