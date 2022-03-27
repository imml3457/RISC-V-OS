#include <common.h>
#include <kprint.h>
#include <imalloc.h>
#include <pci.h>
#include <rng_driver.h>


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
    pci_register_driver(VIRTIO_VENDOR, RNG_DEVICE, virt_rng_drive, virt_rng_drive_init, RNG);
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
                struct PCIdriver* device_driver;
                device_driver = find_driver(ec->vendor_id, ec->device_id);
                if(device_driver != NULL){
                    //need to add switch statement for calling driver_inits
                    device_driver->drive_rng_init(device_driver, capes_l, n_capes);
                    //call the [device]_init_driver needs to be called here
                    //this will handle the virtio shit
                }
                n_capes = 0;
            }
        }
    }
}
