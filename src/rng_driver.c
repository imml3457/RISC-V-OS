#include <rng_driver.h>
#include <kprint.h>
#include <mmu.h>
#include <imalloc.h>


void virt_rng_drive_init(struct PCIdriver* driver, void** capes_list, int n_capes){
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
}

int virt_rng_drive(void* buffer, u16 size){
    u64 physical_addr;
    u32 at_idx;
    u32 mod;

    struct PCIdriver* driver = find_driver(VIRTIO_VENDOR, RNG_DEVICE);
    at_idx = driver->at_idx;
    mod = driver->config->common_cfg->queue_size;

    physical_addr = virt_to_phys(kernel_page_table, (u64)buffer);

    driver->config->desc[at_idx].addr = physical_addr;
    driver->config->desc[at_idx].len = size;
    driver->config->desc[at_idx].flags = VIRTQ_DESC_F_WRITE;
    driver->config->desc[at_idx].next = 0;

    driver->config->available->ring[driver->config->available->idx % mod] = at_idx;

    driver->config->available->idx += 1;

    driver->at_idx = (driver->at_idx + 1) % mod;

    u64 w_bar = find_bar(VIRTIO_VENDOR, RNG_DEVICE, driver->config->notify_cap->cap.bar);
    w_bar &= ~0xFULL;
    w_bar += driver->config->notify_cap->cap.offset;
    *(u32*)(w_bar + driver->config->common_cfg->queue_notify_off * driver->config->notify_cap->notify_off_multiplier) = 0;
    return 1;
}
