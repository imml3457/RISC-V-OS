#include <rng_driver.h>
#include <kprint.h>

void virt_rng_drive_init(struct PCIdriver* driver, void** capes_list, int n_capes){
    for(int i = 0; i < n_capes; i++){
        struct virtio_pci_cap* cap = capes_list[i];
        u64 w_bar = find_bar(driver->vendor, driver->device, cap->bar);
        w_bar &= ~0xFUL;
        w_bar += cap->offset;
        kprint("what is the bar plus offset 0x%X\n", w_bar);
        switch(cap->cfg_type){
            case VIRTIO_CAP_COMMON_CFG:
                driver->common_cfg = (struct virtio_pci_common_cfg*) w_bar;
                break;
            case VIRTIO_CAP_NOTIFY_CFG:
                driver->notify_cap = (struct virtio_pci_notify_cap*) w_bar;
                kprint("what is the notify multi %u\n", driver->notify_cap->notify_off_multiplier);
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

}
