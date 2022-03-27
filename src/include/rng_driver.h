#ifndef __RNG_DRIVER_H__
#define __RNG_DRIVER_H__

#include <virtio.h>
#include <pci.h>


void virt_rng_drive_init(struct PCIdriver*, void**, int);
int virt_rng_drive(void*, u16);

#endif
