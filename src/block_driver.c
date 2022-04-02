#include <block_driver.h>
#include <kprint.h>
#include <mmu.h>
#include <imalloc.h>

void virt_block_drive_init(struct PCIdriver* driver, void** capes_list, int n_capes){
    //setting up the configs
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
                driver->config->device_spec = (struct virtio_blk_config*) w_bar;
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

int find_size(u64 size){
    int rem = size % 512;
    //finding how many sectors
    if(rem != 0){
        return (size / 512) + 1;
    }
    else{
        return size / 512;
    }

}

int virt_block_drive(void* buffer, u64 data_addr, u32 t, u64 size){
    u32 mod;

    struct PCIdriver* driver = find_driver(VIRTIO_VENDOR, BLOCK_DEVICE);
    mod = driver->config->common_cfg->queue_size;

    struct desc_header* header = imalloc(sizeof(struct desc_header));
    u64 blk_size = ((struct virtio_blk_config*)driver->config->device_spec)->blk_size;

    header->type = t;
    header->sector = data_addr / blk_size;

    u8 num_of_sectors = find_size(size);

    u8* data;

    u8* status = imalloc(sizeof(u8));
    if(t == VIRTIO_BLK_T_IN){
        data = imalloc((num_of_sectors * blk_size) * sizeof(u8));
        elems[driver->idx_blk_elems].id = driver->at_idx_desc;
        elems[driver->idx_blk_elems].virt_addr_data = (u64)data;
        elems[driver->idx_blk_elems].virt_addr_status = (u64)status;
        driver->idx_blk_elems++;
    }
    else if(t == VIRTIO_BLK_T_OUT){
        data = imalloc((num_of_sectors * blk_size) * sizeof(u8));
        u64 range;

        //math stuff for calculating offset
        if(size < blk_size){
            range = size;
        }
        else{
            range = data_addr % blk_size;
        }
        u64 i = 0;
        while(i < size){
            data[range + i] = ((u8*)buffer)[i];
            i++;
        }

    }

    //setting the mapping for the blk device


    //setting the header, data, and status descriptors

    u16 header_num = driver->at_idx_desc;
    *status = 69;
    driver->config->desc[driver->at_idx_desc].addr = virt_to_phys(kernel_page_table, (u64)header);
    driver->config->desc[driver->at_idx_desc].len = sizeof(struct desc_header);
    driver->config->desc[driver->at_idx_desc].flags = VIRTQ_DESC_F_NEXT;
    driver->config->desc[driver->at_idx_desc].next = driver->at_idx_desc + 1;

    driver->at_idx_desc++;

    if(t == VIRTIO_BLK_T_IN){
        driver->config->desc[driver->at_idx_desc].addr = virt_to_phys(kernel_page_table, (u64)data);
        driver->config->desc[driver->at_idx_desc].len = num_of_sectors * blk_size * sizeof(u8);
        driver->config->desc[driver->at_idx_desc].flags = VIRTQ_DESC_F_NEXT | VIRTQ_DESC_F_WRITE;
        driver->config->desc[driver->at_idx_desc].next = driver->at_idx_desc + 1;
    }

    else if(t == VIRTIO_BLK_T_OUT){
        driver->config->desc[driver->at_idx_desc].addr = virt_to_phys(kernel_page_table, (u64)data);
        driver->config->desc[driver->at_idx_desc].len = num_of_sectors * blk_size * sizeof(u8);
        driver->config->desc[driver->at_idx_desc].flags = VIRTQ_DESC_F_NEXT;
        driver->config->desc[driver->at_idx_desc].next = driver->at_idx_desc + 1;
    }

    driver->at_idx_desc++;

    driver->config->desc[driver->at_idx_desc].addr = virt_to_phys(kernel_page_table, (u64)status);
    driver->config->desc[driver->at_idx_desc].len = sizeof(u8);
    driver->config->desc[driver->at_idx_desc].flags = VIRTQ_DESC_F_WRITE;

    driver->at_idx_desc++;
    //setting the ring elem to the id of the head of the descriptor
    driver->config->available->ring[driver->config->available->idx % mod] = header_num;

    driver->config->available->idx += 1;

    driver->at_idx = (driver->at_idx + 1) % mod;
    //setting notify offset
    u64 w_bar = find_bar(VIRTIO_VENDOR, BLOCK_DEVICE, driver->config->notify_cap->cap.bar);
    w_bar &= ~0xFULL;
    w_bar += driver->config->notify_cap->cap.offset;
    *(u32*)(w_bar + driver->config->common_cfg->queue_notify_off * driver->config->notify_cap->notify_off_multiplier) = 0;

    return 1;
}


void virt_block_drive_read(void* buffer, u64 addr, u64 size){
    struct PCIdriver* driver = find_driver(VIRTIO_VENDOR, BLOCK_DEVICE);
    int status = virt_block_drive(NULL, addr, VIRTIO_BLK_T_IN, size);

    if(status != 1){
        return;
    }
    //waiting for the irq to finish
    //so the block device can write back to the used ring
    while(driver->at_idx_used == driver->config->used->idx);
    u8* data;
    //getting the id from the used ring
    u32 tempid = driver->config->used->ring[driver->at_idx_used].id;
    for(int i = 0; i < driver->idx_blk_elems; i++){
        //checking my mapping for the corresponding id
        if(tempid == elems[i].id){
             data = (u8*)(elems[i].virt_addr_data);
             break;
        }
    }
    //getting block size (just in case)
    u64 blk_size = ((struct virtio_blk_config*)driver->config->device_spec)->blk_size;
    u64 range;

    //math stuff for calculating offset
    if(size < blk_size){
        range = size;
    }
    else{
        range = addr % blk_size;
    }
    u64 i = 0;
    while(i < size){
        ((u8*)buffer)[i] = data[range + i];
        i++;
    }
}
void virt_block_drive_write(void* buffer, u64 addr, u64 size){
    struct PCIdriver* driver = find_driver(VIRTIO_VENDOR, BLOCK_DEVICE);
    int status = virt_block_drive(buffer, addr, VIRTIO_BLK_T_OUT, size);
    if(status != 1){
        return;
    }
    //waiting for the irq to finish
    //so the block device can write back to the used ring
    while(driver->at_idx_used == driver->config->used->idx);
}
