#ifndef __INPUT_DRIVER_H__
#define __INPUT_DRIVER_H__

#include <virtio.h>
#include <pci.h>
#include <ringbuf.h>

extern struct ring_buffer* ring_buf;

void virt_input_drive_init(struct PCIdriver*, void**, int);
int virt_input_drive();

#endif
