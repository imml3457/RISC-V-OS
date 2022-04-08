#include <input_driver.h>
#include <kprint.h>
#include <mmu.h>
#include <imalloc.h>
#include <input_event_codes.h>
#include <page.h>

#define ALIGN_UP_POT(x, y) (((x)+(y) - 1) & -(y))

struct ring_buffer* ring_buf;
void virt_input_drive_init(struct PCIdriver* driver, void** capes_list, int n_capes, u16 num_dev){
    for(int i = 0; i < n_capes; i++){
        struct virtio_pci_cap* cap = capes_list[i];
        u64 w_bar = find_bar_multiple_devices(driver->vendor, driver->device, cap->bar, driver->bus, driver->device_num);
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
    volatile input_config* inp_config = (input_config*)driver->config->device_spec;
    inp_config->select = VIRTIO_INPUT_CFG_ID_DEVIDS;
    inp_config->subsel = 0;
    driver->config->common_cfg->device_status = VIRTIO_DEV_RESET;
    driver->config->common_cfg->device_status = VIRTIO_DEV_STATUS_ACK;
    driver->config->common_cfg->device_status |= VIRTIO_DEV_STATUS_DRIVER;
    // i need to read features
    driver->config->common_cfg->device_status |= VIRTIO_DEV_STATUS_OK;
    //dont hard code haha
    driver->config->common_cfg->queue_select = 0;
    driver->config->common_cfg->queue_size = 512;
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
    if(num_dev == 1){
        ring_buf = ring_init(ring_buf, 256, RING_BUF_DISCARD);
    }
    u64 num_pages = ALIGN_UP_POT(queue_s * sizeof(struct virtio_input_event), PAGE_SIZE) / PAGE_SIZE;
    volatile struct virtio_input_event* events = imalloc(num_pages * PAGE_SIZE * sizeof(struct virtio_input_event));
    memset((void*)events, 0, sizeof(num_pages * PAGE_SIZE * sizeof(struct virtio_input_event)));

    for(u16 i = 0; i < queue_s; i++){
/*         kprint("event addr: %X\n", (u64)(events + i)); */
        volatile struct virtio_input_event* temp = events + i;
        driver->config->desc[i].addr = virt_to_phys(kernel_page_table, (u64)temp);
        driver->config->desc[i].len = sizeof(struct virtio_input_event);
        driver->config->desc[i].flags = VIRTQ_DESC_F_WRITE;
        driver->config->desc[i].next = 0;
        driver->config->available->ring[driver->at_idx % queue_s] = i;
        driver->at_idx += 1;
    }
    driver->config->available->idx += queue_s;


/*     else if(inp_config->ids.product == EV_ABS){ */
/*         kprint("this is tablet\n"); */
/*     } */


}
