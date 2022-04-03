#include <gpu_driver.h>
#include <kprint.h>
#include <mmu.h>
#include <imalloc.h>

struct membacked_framebuffer *frame_buffer;

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
    frame_buffer = imalloc(sizeof(struct membacked_framebuffer));

}
int virt_gpu_drive(void* packet_type, u64 packet_size, void* ret_buffer, u64 ret_buffer_size, void* response, u64 response_size, u8 if_write){
    u32 mod;

    struct PCIdriver* driver = find_driver(VIRTIO_VENDOR, GPU_DEVICE);
    mod = driver->config->common_cfg->queue_size;

    u32 header_num = driver->at_idx_desc % mod;
    u32 header = header_num;

    driver->config->desc[driver->at_idx_desc % mod].addr = virt_to_phys(kernel_page_table, (u64)packet_type);
    driver->config->desc[driver->at_idx_desc % mod].len = packet_size;
    driver->config->desc[driver->at_idx_desc % mod].flags = 0;

    if(ret_buffer_size != 0 || response_size != 0){
        driver->config->desc[driver->at_idx_desc % mod].flags |= VIRTQ_DESC_F_NEXT;
        driver->config->desc[driver->at_idx_desc % mod].next = (driver->at_idx_desc + 1) % mod;
    }

    driver->at_idx_desc++;

    if(ret_buffer_size != 0){
        driver->config->desc[driver->at_idx_desc % mod].addr = virt_to_phys(kernel_page_table, (u64)ret_buffer);
        driver->config->desc[driver->at_idx_desc % mod].len = ret_buffer_size;

        if(response_size != 0){
            if(if_write == GPU_WRITE){
                driver->config->desc[driver->at_idx_desc % mod].flags = VIRTQ_DESC_F_NEXT;
            }
            else{
                driver->config->desc[driver->at_idx_desc % mod].flags = VIRTQ_DESC_F_NEXT | VIRTQ_DESC_F_WRITE;
            }
            driver->config->desc[driver->at_idx_desc % mod].next = (driver->at_idx_desc + 1) % mod;
        }
        else{
            if(if_write == GPU_WRITE){
                driver->config->desc[driver->at_idx_desc % mod].flags = 0;
            }
            else{
                driver->config->desc[driver->at_idx_desc % mod].flags = VIRTQ_DESC_F_WRITE;
            }
            driver->config->desc[driver->at_idx_desc % mod].next = 0;
        }
        driver->at_idx_desc++;
    }
    if(response != NULL){
        driver->config->desc[driver->at_idx_desc % mod].addr = virt_to_phys(kernel_page_table, (u64)response);
        driver->config->desc[driver->at_idx_desc % mod].len = response_size;
        driver->config->desc[driver->at_idx_desc % mod].flags = VIRTQ_DESC_F_WRITE;
        driver->config->desc[driver->at_idx_desc % mod].next = 0;
        driver->at_idx_desc++;
    }
    driver->config->available->ring[driver->config->available->idx % mod] = header;

    driver->config->available->idx += 1;

    //setting notify offset
    u64 w_bar = find_bar(VIRTIO_VENDOR, GPU_DEVICE, driver->config->notify_cap->cap.bar);
    w_bar &= ~0xFULL;
    w_bar += driver->config->notify_cap->cap.offset;
    *(u32*)(w_bar + driver->config->common_cfg->queue_notify_off * driver->config->notify_cap->notify_off_multiplier) = 0;
    //polling loop
    //this looks similar to your code
    //I understand that this is finding the id of the desired
    //descriptor even though there might be more descriptors on the ring
    //this seems like a good method (and it is similar to what I did on block device)
    u8 correct_desc = 0;
    u32 temp_idx = driver->at_idx_used;
    while(correct_desc == 0){
        while(temp_idx != driver->config->used->idx){
            if(driver->config->used->ring[temp_idx % mod].id == header){
                correct_desc = 1;
            }
            temp_idx++;
        }
    }
    return 1;
}

int virtio_gpu_poll(void* packet_type, u64 packet_size, void* ret_buffer, u64 ret_buffer_size, void* response, u64 response_size, u8 if_write){
    if(if_write == GPU_WRITE){
        virt_gpu_drive(packet_type, packet_size, ret_buffer, ret_buffer_size, response, response_size, if_write);
        return 1;
    }
    else{
        virt_gpu_drive(packet_type, packet_size, ret_buffer, ret_buffer_size, response, response_size, if_write);
        return 1;
    }
    return 0;
}
//i just stole this from you, I understand that it is filling the width and height of a rectangle
void fill_rect(u32 screen_width, u32 screen_height,
               struct pixel *buffer, struct rectangle *rect, struct pixel *fill_color)
{
   u32 top = rect->y;
   u32 bottom = rect->y + rect->height;
   u32 left = rect->x;
   u32 right = rect->x + rect->width;
   u32 row;
   u32 col;
   if (bottom > screen_height) bottom = screen_height;
   if (right > screen_width) right = screen_width;
   for (row = top;row < bottom;row++) {
      for (col = left;col < right;col++) {
          u32 offset = row * screen_width + col;
          buffer[offset] = *fill_color;
      }
   }
}

void stroke_rect(u32 screen_width, u32 screen_height,
                 struct pixel *buffer, struct rectangle *rect,
                 struct pixel *line_color, u32 line_size)
{
   // Top
   struct rectangle top_rect = {rect->x,
                         rect->y,
                         rect->width,
                         line_size};
   fill_rect(screen_width, screen_height, buffer, &top_rect, line_color);
   // Bottom
   struct rectangle bot_rect = {rect->x,
                         rect->height + rect->y,
                         rect->width,
                         line_size};
   fill_rect(screen_width, screen_height, buffer, &bot_rect, line_color);
   // Left
   struct rectangle left_rect = {rect->x,
                          rect->y,
                          line_size,
                          rect->height};
   fill_rect(screen_width, screen_height, buffer, &left_rect, line_color);
   // Right
   struct rectangle right_rect = {rect->x + rect->width,
                           rect->y,
                           line_size,
                           rect->height + line_size};
   fill_rect(screen_width, screen_height, buffer, &right_rect, line_color);
}

int start_gpu(){
    struct control_header header;
    struct display_info_response display_info;
    struct resource_create2d_request request_2d;
    struct virtio_gpu_resource_attach_backing attach_2d;
    struct virtio_gpu_mem_entry mem_entry;
    struct set_scanout_request scanout_request;
    struct virtio_gpu_transfer_to_host_2d transfer_request;
    struct virtio_gpu_resource_flush flush;

    int status = 0;

    header.cont_type = VIRTIO_GPU_CMD_GET_DISPLAY_INFO;

    status = virtio_gpu_poll(&header, sizeof(header), &display_info, sizeof(display_info), NULL, 0, GPU_READ);

    if(status != 1){
        return -1;
    }
    if(display_info.hdr.cont_type != VIRTIO_GPU_RESP_OK_DISPLAY_INFO){
        return -1;
    }
    frame_buffer->width = display_info.displays[0].rect.width;
    frame_buffer->height = display_info.displays[0].rect.height;
    frame_buffer->pix = (struct pixel*)imalloc(sizeof(struct pixel) * frame_buffer->width * frame_buffer->height);


    memset(&request_2d, 0, sizeof(struct resource_create2d_request));

    request_2d.hdr.cont_type = VIRTIO_GPU_CMD_RESOURCE_CREATE_2D;
    request_2d.resource_id = 69;
    request_2d.format =  R8G8B8A8_UNORM;
    request_2d.height = frame_buffer->height;
    request_2d.width = frame_buffer->width;

    header.cont_type = 0;

    status = virtio_gpu_poll(&request_2d, sizeof(request_2d), NULL, 0, &header, sizeof(header), GPU_WRITE);
    if(status != 1){
        return -1;
    }
    if(display_info.hdr.cont_type != VIRTIO_GPU_RESP_OK_DISPLAY_INFO){
        return -1;
    }

    memset(&attach_2d, 0, sizeof(struct virtio_gpu_resource_attach_backing));
    attach_2d.resource_id = 69;
    attach_2d.nr_entries = 1;
    attach_2d.hdr.cont_type = VIRTIO_GPU_CMD_RESOURCE_ATTACH_BACKING;


    mem_entry.addr = virt_to_phys(kernel_page_table, (u64)frame_buffer->pix);
    mem_entry.length = sizeof(struct pixel) * frame_buffer->width * frame_buffer->height;

    header.cont_type = 0;

    status = virtio_gpu_poll(&attach_2d, sizeof(attach_2d), &mem_entry, sizeof(mem_entry), &header, sizeof(header), GPU_WRITE);
    if(status != 1){
        return -1;
    }
    if(display_info.hdr.cont_type != VIRTIO_GPU_RESP_OK_DISPLAY_INFO){
        return -1;
    }

    memset(&scanout_request, 0, sizeof(struct set_scanout_request));

    scanout_request.hdr.cont_type = VIRTIO_GPU_CMD_SET_SCANOUT;
    scanout_request.rect.width = frame_buffer->width;
    scanout_request.rect.height = frame_buffer->height;
    scanout_request.resource_id = 69;
    scanout_request.scanout_id = 0;

    header.cont_type = 0;

    status = virtio_gpu_poll(&scanout_request, sizeof(scanout_request), NULL, 0, &header, sizeof(header), GPU_WRITE);
    if(status != 1){
        return -1;
    }
    if(display_info.hdr.cont_type != VIRTIO_GPU_RESP_OK_DISPLAY_INFO){
        return -1;
    }

    struct rectangle rect1 = {0, 0, frame_buffer->width, frame_buffer->height};
    struct rectangle rect2 = {100, 100, frame_buffer->width - 150, frame_buffer->height - 150};
    struct pixel p1 = {255, 100, 50, 255};
    struct pixel p2 = {50, 0, 255, 255};

    fill_rect(frame_buffer->width, frame_buffer->height, frame_buffer->pix, &rect1, &p1);
    stroke_rect(frame_buffer->width, frame_buffer->height, frame_buffer->pix, &rect2, &p2, 10);

    memset(&transfer_request, 0, sizeof(struct virtio_gpu_transfer_to_host_2d));
    transfer_request.hdr.cont_type = VIRTIO_GPU_CMD_TRANSFER_TO_HOST_2D;
    transfer_request.r.width = frame_buffer->width;
    transfer_request.r.height = frame_buffer->height;
    transfer_request.resource_id = 69;

    header.cont_type = 0;

    status = virtio_gpu_poll(&transfer_request, sizeof(transfer_request), NULL, 0, &header, sizeof(header), GPU_WRITE);
    if(status != 1){
        return -1;
    }
    if(display_info.hdr.cont_type != VIRTIO_GPU_RESP_OK_DISPLAY_INFO){
        return -1;
    }

    memset(&flush, 0, sizeof(struct virtio_gpu_resource_flush));

    flush.hdr.cont_type = VIRTIO_GPU_CMD_RESOURCE_FLUSH;
    flush.r.width = frame_buffer->width;
    flush.r.height = frame_buffer->height;
    flush.resource_id = 69;

    header.cont_type = 0;

    status = virtio_gpu_poll(&flush, sizeof(flush), NULL, 0, &header, sizeof(header), GPU_WRITE);
    if(status != 1){
        return -1;
    }
    if(display_info.hdr.cont_type != VIRTIO_GPU_RESP_OK_DISPLAY_INFO){
        return -1;
    }

    return 0;
}
