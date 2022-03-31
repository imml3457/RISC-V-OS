#include <block.h>
#include <block_driver.h>

void* dsk_read(u64 addr_block, u64 size){
    return virt_block_drive(addr_block, VIRTIO_BLK_T_IN, size);
}
