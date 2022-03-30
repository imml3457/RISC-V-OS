#ifndef __BLOCK_DRIVER_H__
#define __BLOCK_DRIVER_H__

#include <virtio.h>
#include <pci.h>


void virt_block_drive_init(struct PCIdriver*, void**, int);
int virt_block_drive();

#endif
