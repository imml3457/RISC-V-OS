#ifndef __PCI_H__
#define __PCI_H__

struct ecamheader {
    u16 vendor_id;
    u16 device_id;
    u16 command_reg;
    u16 status_reg;
    u8 revision_id;
    u8 prog_if;
    union {
        u16 class_code;
        struct {
            u8 class_subcode;
            u8 class_basecode;
        };
    };
    u8 cacheline_size;
    u8 latency_timer;
    u8 header_type;
    u8 bist;
    union {
        struct {
            u32 bar[6];
            u32 cardbus_cis_pointer;
            u16 sub_vendor_id;
            u16 sub_device_id;
            u32 expansion_rom_addr;
            u8  capes_pointer;
            u8  reserved0[3];
            u32 reserved1;
            u8  interrupt_line;
            u8  interrupt_pin;
            u8  min_gnt;
            u8  max_lat;
        } type0;
        struct {
            u32 bar[2];
            u8  primary_bus_no;
            u8  secondary_bus_no;
            u8  subordinate_bus_no;
            u8  secondary_latency_timer;
            u8  io_base;
            u8  io_limit;
            u16 secondary_status;
            u16 memory_base;
            u16 memory_limit;
            u16 prefetch_memory_base;
            u16 prefetch_memory_limit;
            u32 prefetch_base_upper;
            u32 prefetch_limit_upper;
            u16 io_base_upper;
            u16 io_limit_upper;
            u8  capes_pointer;
            u8  reserved0[3];
            u32 expansion_rom_addr;
            u8  interrupt_line;
            u8  interrupt_pin;
            u16 bridge_control;
        } type1;
        struct {
            u32 reserved0[9];
            u8  capes_pointer;
            u8  reserved1[3];
            u32 reserved2;
            u8  interrupt_line;
            u8  interrupt_pin;
            u8  reserved3[2];
        } common;
    };
};

struct capability{
    u8 id;
    u8 next;
};

volatile struct ecamheader *get_ecam(u8, u8, u8, u16);

void initpci(void);
void initbar(void);


#endif
