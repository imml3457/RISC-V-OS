#ifndef __INPUT_DRIVER_H__
#define __INPUT_DRIVER_H__

#include <virtio.h>
#include <pci.h>
#include <ringbuf.h>

extern struct ring_buffer* ring_buf;

typedef enum virtio_input_config_select {
    VIRTIO_INPUT_CFG_UNSET = 0x00,
    VIRTIO_INPUT_CFG_ID_NAME = 0x01,
    VIRTIO_INPUT_CFG_ID_SERIAL = 0x02,
    VIRTIO_INPUT_CFG_ID_DEVIDS = 0x03,
    VIRTIO_INPUT_CFG_PROP_BITS = 0x10,
    VIRTIO_INPUT_CFG_EV_BITS = 0x11,
    VIRTIO_INPUT_CFG_ABS_INFO = 0x12,
} InputConfigSelect;
typedef struct virtio_input_absinfo {
    uint32_t min;
    uint32_t max;
    uint32_t fuzz;
    uint32_t flat;
    uint32_t res;
} InputAbsInfo;
typedef struct virtio_input_devids {
    uint16_t bustype;
    uint16_t vendor;
    uint16_t product;
    uint16_t version;
} InputDevIds;
typedef struct virtio_input_config {
    uint8_t select;
    uint8_t subsel;
    uint8_t size;
    uint8_t reserved[5];
    union {
        char string[128];
        uint8_t bitmap[128];
        struct virtio_input_absinfo abs;
        struct virtio_input_devids ids;
    };
}input_config;

struct virtio_input_event {
    uint16_t type;
    uint16_t code;
    uint32_t value;
};

void virt_input_drive_init(struct PCIdriver*, void**, int, u16);
int virt_input_drive(struct PCIdriver*);

#endif
