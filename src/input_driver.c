#include <input_driver.h>
#include <kprint.h>
#include <mmu.h>
#include <imalloc.h>
#include <input_event_codes.h>
#include <page.h>

struct ring_buffer* ring_buffer;

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



void virt_input_drive_init(struct PCIdriver* driver, void** capes_list, int n_capes){
    for(int i = 0; i < n_capes; i++){
        struct virtio_pci_cap* cap = capes_list[i];
        u64 w_bar = find_bar(driver->vendor, driver->device, cap->bar);
        w_bar &= ~0xFULL;
        w_bar += cap->offset;
        switch(cap->cfg_type){
            case VIRTIO_CAP_COMMON_CFG:;
                driver->config->common_cfg = (struct virtio_pci_common_cfg*) w_bar;
                break;
            case VIRTIO_CAP_NOTIFY_CFG:
                driver->config->notify_cap = (struct virtio_pci_notify_cap*) cap;
                break;
            case VIRTIO_CAP_ISR_CFG:
                driver->config->isr_cap = (struct virtio_pci_isr_cap*) w_bar;
                break;
            case VIRTIO_CAP_DEVICE_CFG:
                driver->config->device_spec = (input_config*) w_bar;
                break;
            case VIRTIO_CAP_PCI_CFG:
                break;
            default:
                break;
        }
    }
    driver->config->common_cfg->device_status = VIRTIO_DEV_RESET;
    driver->config->common_cfg->device_status = VIRTIO_DEV_STATUS_ACK;
    driver->config->common_cfg->device_status |= VIRTIO_DEV_STATUS_DRIVER;
    // i need to read features
    driver->config->common_cfg->device_status |= VIRTIO_DEV_STATUS_OK;
    //dont hard code haha
    driver->config->common_cfg->queue_select = 0;
    u16 queue_s = driver->config->common_cfg->queue_size;

    u64 temp_virt_addr = (u64)imalloc(16 * queue_s);
    driver->config->desc = (struct virtq_desc*) temp_virt_addr;
    driver->config->common_cfg->queue_desc = virt_to_phys(kernel_page_table, temp_virt_addr);

    temp_virt_addr = (u64)imalloc(6 + 2 * queue_s);
    driver->config->available = (struct virtq_avail*)temp_virt_addr;
    driver->config->common_cfg->queue_driver = virt_to_phys(kernel_page_table, temp_virt_addr);

    temp_virt_addr = (u64)imalloc(6 + 8 * queue_s);
    driver->config->used = (struct virtq_used*) temp_virt_addr;
    driver->config->common_cfg->queue_device = virt_to_phys(kernel_page_table, temp_virt_addr);

    driver->config->common_cfg->queue_enable = 1;

    driver->config->common_cfg->device_status |= VIRTIO_DEV_DRIVER_OK;
    ring_buffer = ring_init(ring_buffer, 256, RING_BUF_DISCARD);

    struct virtio_input_event* events = page_cont_falloc((((queue_s + PAGE_SIZE) - 1) & -(PAGE_SIZE)) /  PAGE_SIZE);

    for(u16 i = 0; i < queue_s; i++){
        struct virtio_input_event* temp = events + i;
        driver->config->desc[i].addr = (u64)temp;
        driver->config->desc[i].len = sizeof(struct virtio_input_event);
        driver->config->desc[i].flags = VIRTQ_DESC_F_WRITE;
        driver->config->desc[i].next = 0;
        driver->config->available->ring[driver->config->available->idx % queue_s] = i;
        driver->at_idx += 1;
    }
    driver->config->available->idx = queue_s;
}
