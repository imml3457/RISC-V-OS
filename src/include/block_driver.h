#ifndef __BLOCK_DRIVER_H__
#define __BLOCK_DRIVER_H__

#include <virtio.h>
#include <pci.h>


void virt_block_drive_init(struct PCIdriver*, void**, int);
int virt_block_drive(u64, u32, u64);
int virt_block_drive_read(u64, u64);

struct desc_header{
    u32 type;
    u32 reserved;
    u64 sector;
};

#define VIRTIO_BLK_T_IN 0
#define VIRTIO_BLK_T_OUT 1
#define VIRTIO_BLK_T_FLUSH 4
#define VIRTIO_BLK_T_DISCARD 11
#define VIRTIO_BLK_T_WRITE_ZEROES 13

#define VIRTIO_BLK_S_OK 0
#define VIRTIO_BLK_S_IOERR 1
#define VIRTIO_BLK_S_UNSUPP 2

#endif
