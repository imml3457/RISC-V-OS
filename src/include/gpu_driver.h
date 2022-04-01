#ifndef __GPU_DRIVER_H__
#define __GPU_DRIVER_H__

#include <virtio.h>
#include <pci.h>


void virt_gpu_drive_init(struct PCIdriver*, void**, int);
int virt_gpu_drive();

#endif
