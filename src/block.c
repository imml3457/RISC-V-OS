#include <block.h>
#include <block_driver.h>

int dsk_read(u64 addr_block, u64 size){
    return virt_block_drive_read(addr_block, size);
}
