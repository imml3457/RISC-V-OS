#include <kprint.h>
#include <imalloc.h>
#include <pci.h>

struct driver_list* dlist;

struct PCIdriver* find_driver(u16 vendor, u16 device){
    struct driver_list *list;
    for(list = dlist; list!=NULL; list = list->next){
        if(list->driver.vendor == vendor && list->driver.device == device){
            return &list->driver;
        }
    }
    return NULL;
}

void pci_register_driver(u16 vendor, u16 device, void* driver_fn, void* driver_init_fn, int type){
    if(find_driver(vendor, device) != NULL){
        kprint("pcie driver registration: Already registered");
        kprint(" Vendor ID: 0x%x, Device ID 0x%x\n", vendor, device);
    }
    else{
        struct driver_list *l;
        l = imalloc(sizeof(struct driver_list));
        l->driver.vendor = vendor;
        l->driver.device = device;
        switch(type){
            case RNG:
                l->driver.drive_rng = driver_fn;
                l->driver.drive_rng_init = driver_init_fn;
                break;
            default:
                break;
        }
        l->next = dlist;
        dlist = l;
    }
}

void virt_rng_drive(){
    kprint("in rng_driver\n");
}
