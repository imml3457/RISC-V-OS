#include <gpu_driver.h>
#include <kprint.h>
#include <mmu.h>
#include <imalloc.h>

#define VIRTIO_GPU_EVENT_DISPLAY   1

struct virtio_gpu_config {
   u32  events_read;
   u32  events_clear;
   u32  num_scanouts;
   u32  reserved;
};

void virt_gpu_drive_init(struct PCIdriver* driver, void** capes_list, int n_capes){
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
                driver->config->device_spec = (struct VirtioGpuConfig*) w_bar;
                break;
            case VIRTIO_CAP_PCI_CFG:
                break;
            default:
                break;
        }
    }

    //activating the device
    driver->config->common_cfg->device_status = VIRTIO_DEV_RESET;
    driver->config->common_cfg->device_status = VIRTIO_DEV_STATUS_ACK;
    driver->config->common_cfg->device_status |= VIRTIO_DEV_STATUS_DRIVER;
    driver->config->common_cfg->device_status |= VIRTIO_DEV_STATUS_OK;
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
int virt_gpu_drive(){
    return 1;
}
