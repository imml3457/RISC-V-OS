#include <common.h>
#include <kprint.h>
#include <imalloc.h>
#include <pci.h>
#include <rng_driver.h>
#include <block_driver.h>
#include <gpu_driver.h>
#include <input_driver.h>
#include <ringbuf.h>

int num_inp_devices = 1;
struct blk_elem elems[1024];

u64 find_bar(u16 vendor, u16 d, u8 bar){
    for(int bus = 0; bus < 256; bus++){
        for(int device = 0; device < 32; device++){
            volatile struct ecamheader *ec = get_ecam(bus, device, 0, 0);
            if(ec->vendor_id == vendor && ec->device_id == d){
                //it is a u64 bar
                if(((ec->type0.bar[bar] & 0x7) >> 1) != 0){
                    return *((u64*)&ec->type0.bar[bar]);
                }
                else{
                    u32 b = ec->type0.bar[bar];
                    return b;
                }
            }
        }
    }
    return 0;
}
u64 find_bar_multiple_devices(u16 vendor, u16 d, u8 bar, u16 bus, u16 device){
        volatile struct ecamheader *ec = get_ecam(bus, device, 0, 0);
        if(ec->vendor_id == vendor && ec->device_id == d){
            //it is a u64 bar
            if(((ec->type0.bar[bar] & 0x7) >> 1) != 0){
                return *((u64*)&ec->type0.bar[bar]);
            }
            else{
                u32 b = ec->type0.bar[bar];
                return b;
            }
        }
    return 0;
}


volatile struct ecamheader *get_ecam(u8 bus, u8 device, u8 function, u16 regs){
    u64 bus_64 = bus & 0xff;
    u64 device_64 = device & 0x1f;
    u64 function_64 = function & 0x7;
    u64 register_64 = regs & 0x3ff;
    return (struct ecamheader*)(ECAM_BASE |
            (bus_64 << 20) |
            (device_64 << 15) |
            (function_64 << 12) |
            (register_64 << 2));
}

void initpci(void){

    //setting up the type ones
    //i am going to reform this code so i just have one single loop
    int bus;
    int device;
    u32 prev_bar_size = BRIDGE_BASE;
    for (bus = 0;bus < 256;bus++) {
        for (device = 0;device < 32;device++) {
            volatile struct ecamheader *ec = get_ecam(bus, device, 0, 0);
            if (ec->vendor_id == 0xffff) continue;
            if(ec->header_type == 1){
                static int pci_bus_no = 1;
/*                 u64 addrstart = 0x40000000 | ((u64)pci_bus_no << 20); */
/*                 u64 addrend = addrstart + ((1 << 20) - 1); */
                ec->command_reg = (1<<1) | (1<<2); // bits 1 and 2
                //handling the memory range and prefetch range
                ec->type1.memory_base = 0x4000;
                ec->type1.memory_limit = 0x4fff;
                ec->type1.prefetch_memory_limit = 0x4fff;
                ec->type1.prefetch_memory_base = 0x4000;
                //setting the bus numbers
                ec->type1.primary_bus_no = bus;
                ec->type1.secondary_bus_no = pci_bus_no;
                ec->type1.subordinate_bus_no = pci_bus_no;
                pci_bus_no += 1;
            }
            else if(ec->header_type == 0){
                ec->command_reg = (1 << 1);
                for(int i = 0; i < 6; i++){
                    int check_64_bar = (ec->type0.bar[i] & 0x00000007) >> 1;
                    //it is 32 bits
                    if(check_64_bar == 0){
                        u32 tmp_addr = 0;
                        //writing 0 in the command register
                        ec->command_reg &= 0xFFFD;
                        ec->type0.bar[i] = 0xffffffff;
                        u32 bar_size = -(ec->type0.bar[i] & ~0xFUL);
                        if(bar_size != 0){
                            //giving the memory to the bar
                            tmp_addr = (prev_bar_size + (bar_size - 1)) & (~(bar_size - 1));
                            ec->type0.bar[i] = tmp_addr;
                        }
                        prev_bar_size += bar_size;
                        ec->command_reg |= (1 << 1);
                    }
                    else{
                        //if it is 64 bit
                        //it cannot be bar 6
                        if(i != 5){
                            ec->command_reg &= 0xFFFD;
                            *((u64*)&ec->type0.bar[i]) = 0xffffffffffffffff;
                            u64 bar_size = -(*((u64*)&ec->type0.bar[i]) & ~0xFULL);
                            if(bar_size != 0){
                                u64 tmp_addr = ((u64)prev_bar_size + (bar_size - 1)) & (~(bar_size - 1));
                                *((u64*)&ec->type0.bar[i]) = tmp_addr;
                            }
                            prev_bar_size += bar_size;
                            ec->command_reg |= (1 << 1);
                            i++;
                        }
                    }
                }
                prev_bar_size += 0x1000000;
                prev_bar_size &= ~(0xffffff);
            }
        }
    }
    pci_register_driver(VIRTIO_VENDOR, RNG_DEVICE, virt_rng_drive, virt_rng_drive_init, RNG, 0);
    pci_register_driver(VIRTIO_VENDOR, BLOCK_DEVICE, virt_block_drive, virt_block_drive_init, BLOCK, 0);
    pci_register_driver(VIRTIO_VENDOR, GPU_DEVICE, virt_gpu_drive, virt_gpu_drive_init, GPU, 0);
    pci_register_driver(VIRTIO_VENDOR, INPUT_DEVICE, NULL, virt_input_drive_init, INPUT, 1);
    pci_register_driver(VIRTIO_VENDOR, INPUT_DEVICE, NULL, virt_input_drive_init, INPUT, 2);

}


void pci_set_capes(){
    void* capes_l[256];
    int n_capes = 0;
    for (int bus = 0;bus < 256;bus++) {
        for (int device = 0;device < 32;device++) {
            volatile struct ecamheader *ec = get_ecam(bus, device, 0, 0);
            if (ec->vendor_id == 0xffff) continue;
            if(ec->header_type == 0){
                if(ec->status_reg & (1 << 4)){
                    u8 cape_next = ec->common.capes_pointer;
                    while(cape_next != 0){
                        void* cape_addr = (void*)get_ecam(bus, device, 0, 0) + cape_next;
                        struct capability *cape = cape_addr;
                        switch (cape->id){
                            case 0x09:
                                capes_l[n_capes] = cape_addr;
                                n_capes++;
                                break;
                            case 0x10:
                                break;
                            default:
                                break;
                        }
                        cape_next = cape->next;
                    }
                }
                struct PCIdriver* temp;
                struct driver_list *list;
                for(list = dlist; list!=NULL; list = list->next){
                    if(list->driver->vendor == ec->vendor_id && list->driver->device == ec->device_id){
                        if(list->driver->type == INPUT){
                            if(list->driver->num_inputs == num_inp_devices){
                                temp = list->driver;
                            }
                        }
                        else{
                            temp = list->driver;
                        }
                    }
                }
                temp->irq = 32 + ((bus + device) % 4);
                switch(temp->type){
                    case RNG:
                        temp->bus = bus;
                        temp->device_num = device;
                        temp->drive_rng_init(temp, capes_l, n_capes);
                        break;
                    case BLOCK:
                        temp->bus = bus;
                        temp->device_num = device;
                        temp->drive_block_init(temp, capes_l, n_capes);
                        break;
                    case GPU:
                        temp->bus = bus;
                        temp->device_num = device;
                        temp->drive_gpu_init(temp, capes_l, n_capes);
                        break;
                    case INPUT:
                        temp->bus = bus;
                        temp->device_num = device;
                        temp->drive_input_init(temp, capes_l, n_capes, num_inp_devices);
                        num_inp_devices++;
                        break;
                }
                n_capes = 0;
            }
        }
    }
}
static inline u64 pack_event_input(u64 event_addr){
    return *((const u64*)event_addr);
}


int pci_irq_handle(u64 irq){
    //handling irq
    struct PCIdriver* temp;
    struct driver_list *list;
    for(list = dlist; list!=NULL; list = list->next){
        temp = list->driver;
        if(temp->irq == irq){
            if(temp->config->isr_cap->queue_interrupt){
                if(temp->type == GPU){
                    if(((struct virtio_gpu_config*)temp->config->device_spec)->events_read != 0){
                        ((struct virtio_gpu_config*)temp->config->device_spec)->events_clear = 1;
                        temp->gpu_display_change = 1;
                        break;

                    }
                }
                while(temp->config->used->idx != temp->at_idx_used){
                    if(temp->type == INPUT){
                        u16 queue_s = temp->config->common_cfg->queue_size;

                        u32 idx = temp->at_idx_used % queue_s;
                        u32 id = temp->config->used->ring[idx].id;
                        temp->at_idx_used++;

                        ring_push(ring_buf, pack_event_input(temp->config->desc[id].addr));
                        temp->config->available->idx++;
                    }
                    else{
                        temp->at_idx_used++;
                    }
                }
/*                 return 0; */
            }
        }
    }
    return 0;
}
