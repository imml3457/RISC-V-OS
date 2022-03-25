#include <rng_driver.h>
#include <kprint.h>


void virt_rng_drive_init(struct PCIdriver* driver, void** capes_list, int n_capes){
    for(int i = 0; i < n_capes; i++){
        struct virtio_pci_cap* cap = capes_list[i];
        u64 w_bar = find_bar(driver->vendor, driver->device, cap->bar);
        w_bar &= ~0xFUL;
        w_bar += cap->offset;
        kprint("what is the bar %X\n", w_bar);
        switch(cap->cfg_type){
            case VIRTIO_CAP_COMMON_CFG:
                driver->common_cfg = (struct virtio_pci_common_cfg*) w_bar;
                break;
            case VIRTIO_CAP_NOTIFY_CFG:
                driver->notify_cap = (struct virtio_pci_notify_cap*) w_bar;
                break;
            case VIRTIO_CAP_ISR_CFG:
                driver->isr_cap = (struct virtio_pci_isr_cap*) w_bar;
                break;
            case VIRTIO_CAP_DEVICE_CFG:
                break;
            case VIRTIO_CAP_PCI_CFG:
                break;
            default:
                break;
        }
    }
    driver->common_cfg->device_status = VIRTIO_DEV_RESET;
    kprint("what is the device status %x\n", driver->common_cfg->device_status);
    driver->common_cfg->device_status = VIRTIO_DEV_STATUS_ACK;
    driver->common_cfg->device_status |= VIRTIO_DEV_STATUS_DRIVER;
    // i need to read features
    driver->common_cfg->device_status |= VIRTIO_DEV_STATUS_OK;
    //dont hard code haha
    driver->common_cfg->queue_select = 0;
    u16 queue_s = driver->common_cfg->queue_size;
}
