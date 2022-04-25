#include <utils.h>
#include <kprint.h>
#include <common.h>
#include <csr.h>
#include <page.h>
#include <mmu.h>
#include <pci.h>
#include <sbi.h>
#include <rng.h>
#include <block.h>
#include <input_driver.h>
#include <imalloc.h>
#include <ringbuf.h>
#include <elf.h>

#define IS_LEAP(x) (!((x) & 0x3))
#define _28_days 2419200000000000
#define _29_days 2505600000000000
#define _30_days 2592000000000000
#define _31_days 2678400000000000


u64 months[] = {
                _31_days, //jan
                _28_days, //feb
                _31_days, //mar
                _30_days, //apr
                _31_days, //may
                _30_days, //june
                _31_days, //july
                _31_days, //august
                _30_days, //sept
                _31_days, //oct
                _30_days, //novem
                _31_days //d
};

char* month_name[] = {
                "January",
                "February",
                "March",
                "April",
                "May",
                "June",
                "July",
                "August",
                "September",
                "October",
                "November",
                "December"
};
int strcmp(char* str1, char* str2){
    //error checking the strings
    while(*str1 != '\0' && *str2 != '\0'){
        if(*str1 != *str2){
            //return 1 if they is discrepancy
            return *str1 - *str2;
        }
        str1++;
        str2++;
    }

    if(!(*str1 | *str2)){
        return 0;
    }
    return -1;
}

int strncmp(char* str1, char* str2, u32 n){
    while (n && *str1 && ( *str1 == *str2)){
        str1++;
        str2++;
        n--;
    }

    if(n == 0){
        return 0;
    }
    else{
        return *str1 - *str2;
    }
}

int strcpy(char* str1, char* str2){
    while(*str2 != '\0'){
        *str1 = *str2;
        str1++;
        str2++;
    }
    *str1 = '\0';
    return 0;
}

int strlen(const char* str){
    int len = 0;
    while(str[len]){
        len++;
    }
    return len;
}

int atoi(const char* str){
    int result = 0;

    int sign = 1;

    int i = 0;

    if(str[0] == '-'){
        sign = -1;
        i++;
    }

    for (; str[i] != '\0'; i++){
        result = result * 10 + str[i] - '0';
    }

    return sign * result;
}

void* memset(void* dst, u32 c, u64 size){
    void* dst_head = dst;
    u8 tmp[] = {
                    (u8)c,
                    (u8)c,
                    (u8)c,
                    (u8)c,
                    (u8)c,
                    (u8)c,
                    (u8)c,
                    (u8)c
    };

    u64 rem = size & 0x7;
    u32 i;
    //fast as [inaudible] boy
    for(i = 0; i < size - rem; i+=8){
        *(u64*)(dst + i) = *(u64*)((void*)tmp);
    }
    while(rem > 0){
        *(u8*)(dst) = (u8)c;
        dst++;
        rem--;
    }
    return dst_head;
}
void* memcpy(void* dst, void* src, u64 size){
    u64 rem = size & 0x7;
    u32 i;
    //fast as [inaudible] boy
    for(i = 0; i < size - rem; i+=8){
        *(u64*)(dst + i) = *(u64*)(src + i);
    }
    while(rem > 0){
        *(u8*)(dst) = *(u8*)(src);
        dst++;
        src++;
        rem--;
    }
    return dst;
}

void memmove(void* dst, void* src, u64 size){
    char* csrc = (char*)src;
    char* cdest = (char*)dst;

    char* temp = imalloc(sizeof(char) * size);

    for(int i = 0; i < size; i++){
        *(temp + i) = *(csrc + i);
    }

    for(int i = 0; i < size; i++){
        *(cdest + i) = *(temp + i);
    }

    imfree(temp);
}

static inline s32 is_space(s32 c) {
    unsigned char d = c - 9;
    return (0x80001FU >> (d & 31)) & (1U >> (d >> 5));
}

static inline s32 is_digit(s32 c) {
    return (u32)(('0' - 1 - c) & (c - ('9' + 1))) >> (sizeof(c) * 8 - 1);
}

static inline s32 is_alpha(s32 c) {
    return (u32)(('a' - 1 - (c | 32)) & ((c | 32) - ('z' + 1))) >> (sizeof(c) * 8 - 1);
}

static inline s32 is_alnum(s32 c) {
    return is_alpha(c) || is_digit(c);
}

static inline s32 is_print(s32 c) {
    return (((u32)c) - 0x20) < 0x5f;
}

static inline u64 bswap64(u64 val) {
  return
      ((val & 0xFF00000000000000ULL) >> 56ULL) |
      ((val & 0x00FF000000000000ULL) >> 40ULL) |
      ((val & 0x0000FF0000000000ULL) >> 24ULL) |
      ((val & 0x000000FF00000000ULL) >>  8ULL) |
      ((val & 0x00000000FF000000ULL) <<  8ULL) |
      ((val & 0x0000000000FF0000ULL) << 24ULL) |
      ((val & 0x000000000000FF00ULL) << 40ULL) |
      ((val & 0x00000000000000FFULL) << 56ULL);
}
void hexdump(const u8 *bytes, u32 n_bytes) {
    u32  i;
    u32  j;
    char c;
    char s[17];

    i = 0;

    while (n_bytes >= 16) {
        for (j = 0; j < 16; j += 1) {
            c = bytes[i + j];
            if (is_print(c) && (c == ' ' || !is_space(c))) {
                s[j] = c;
            } else {
                s[j] = '.';
            }
        }
        s[16] = 0;

        kprint("%b%016X%_",   bswap64(*(u64*)(void*)(bytes + i)));
        i       += 8;
        n_bytes -= 8;
        kprint("%b%016X%_    ", bswap64(*(u64*)(void*)(bytes + i)));
        i       += 8;
        n_bytes -= 8;

        kprint("%g%-16s%_\n", s);

    }

    if (n_bytes > 0) {
        for (j = 0; j < n_bytes; j += 1) {
            c = bytes[i + j];
            if (is_print(c) && !is_space(c)) {
                s[j] = c;
            } else {
                s[j] = '.';
            }
        }
        s[j] = 0;

        while (n_bytes > 0) {
            kprint("%b%02x%_", bytes[i]);
            i       += 1;
            n_bytes -= 1;
        }

        while (i & 15) {
            kprint("  ");
            i += 1;
        }

        kprint("    %g%-16s%_\n", s);
    }
}
u64 get_nano_time(){
    volatile u32* RTC_BASE = (void*)0x101000UL;
    volatile u32* RTC_STATUS = ((void*)RTC_BASE) + 0x04;
    mmu_map(kernel_page_table, (u64)RTC_BASE, (u64)RTC_BASE, PAGE_SIZE, read);
    u64 low = (*RTC_BASE);
    u64 high = (*RTC_STATUS);
    u64 time = (high << 32) | low;
    return time;
}

unix_time get_unix_time(u64 time){
    unix_time u_time;
    u64 year_time;
    u64 running_year = 0;

    //hard code eastern time
    time -= 18000000000000;
    //year
    while(1){
        year_time = IS_LEAP(1970 + running_year)? NS_LEAP : NS_YEAR;

        if(time < year_time) { break; }

        time -= year_time;
        running_year += 1;
    }
    u_time.year = 1970 + running_year;

    months[1] = IS_LEAP(u_time.year)? _28_days : _29_days;
    u32 i;
    for(i = 0; i < 12; i++){
        if(time < months[i]){
            u_time.month = i;
            break;
        }
        time -= months[i];
    }
    i = 1;
    while(1){
        if(time < NS_DAY){
            u_time.day = i;
            break;
        }
        i++;
        time -= NS_DAY;
    }

    u_time.hour = time / 3600000000000;
    time -= u_time.hour * 3600000000000;
    u_time.min = time / 60000000000;
    time -= u_time.min * 60000000000;
    u_time.sec = time / 1000000000;
    time -= u_time.sec * 1000000000;

    return u_time;
}

void print_unix_time(){
    u64 time = get_nano_time();
    unix_time u_time = get_unix_time(time);
    if(u_time.min < 10){
        kprint("%U-%s-%U %U:0%U", u_time.year, month_name[u_time.month], u_time.day, u_time.hour, u_time.min, u_time.sec);
    }
    else{
        kprint("%U-%s-%U %U:%U", u_time.year, month_name[u_time.month], u_time.day, u_time.hour, u_time.min, u_time.sec);
    }
    if(u_time.sec < 10){
        kprint(".0%U", u_time.sec);
    }
    else{
        kprint(".%U", u_time.sec);
    }
}

ATTR_NAKED_NORET
void test_hart(void){
    u64 hart;
    CSR_READ(hart, "sscratch");
/*     kprint("HART %U: Started at %U, I am sleepy Mr. Stark!\n", hart, get_nano_time() / 1000000000UL); */
    sbi_stop_hart();
    kprint("I shouldn't be here\n");
    while (1) { WFI();}

}


void exec_cmd(char* cmd){

    //for executing command
    if(strcmp(cmd, "test") == 0){
        kprint("this is a test!\n");
    }
    else if(strcmp(cmd, "gettime()") == 0){
        u64 tm;
        asm volatile("rdtime %0" : "=r"(tm));
        kprint("%U\n", tm);
    }
    else if(strcmp(cmd, "gettimeofday()") == 0){
        print_unix_time();
        kprint("\n");
    }
    else if(strcmp(cmd, "status") == 0){
        int i;
        u64 stat;
        for(i = 0; i < 8; i++){
            stat = sbi_hart_status(i);
            if(stat == 0){
                kprint("HART %d: Invalid\n", i);
            }
            else if(stat == 1){
                kprint("HART %d: Stopped\n", i);
            }
            else if(stat == 2){
                kprint("HART %d: Stopping\n", i);
            }
            else if(stat == 3){
                kprint("HART %d: Started\n", i);
            }
            else if(stat == 4){
                kprint("HART %d: Starting\n", i);
            }
        }
    }
    else if(strcmp(cmd, "systemoff") == 0){
        sbi_system_off();
    }
    else if(strcmp(cmd, "lspci") == 0){
        u64 bus;
        u64 device;
        // There are a MAXIMUM of 256 busses
        // although some implementations allow for fewer.
        // Minimum # of busses is 1
        for (bus = 0;bus < 256;bus++) {
            for (device = 0;device < 32;device++) {
                // EcamHeader is defined below
                volatile struct ecamheader *ec = get_ecam(bus, device, 0, 0);
                // Vendor ID 0xffff means "invalid"

                if (ec->vendor_id == 0xffff) continue;
                // If we get here, we have a device.
                kprint("Device at bus %d, device %d (MMIO @ 0x%X), class: 0x%x\n",
                        bus, device, ec, ec->class_code);
                kprint("   Device ID    : 0x%x, Vendor ID    : 0x%x\n",
                        ec->device_id, ec->vendor_id);
            }
        }
    }
    else if(strcmp(cmd, "randbytes") == 0){
        char bytes[5] = {0};
        rng(bytes, sizeof(bytes));
        kprint("%02x %02x %02x %02x %02x\n", bytes[0], bytes[1], bytes[2], bytes[3], bytes[4]);
    }
    else if(strcmp(cmd, "read") == 0){
        u8* bytes = imalloc(4094);
        dsk_read(bytes, 4146, 4094);
        hexdump(bytes, 4094);
/*         kprint("read data: %s\n", bytes); */
    }
    else if(strcmp(cmd, "write") == 0){
        u8* bytes = imalloc(1024 * sizeof(u8));
        for(int i = 0; i < 1024; i+=4){
            bytes[i] = 't';
            bytes[i+1] = 'e';
            bytes[i+2] = 's';
            bytes[i+3] = 't';
        }

        dsk_write(bytes, 3072004, 1024);
        kprint("written data!\n");
    }

    else if(strcmp(cmd, "ring_pop") == 0){
        struct virtio_input_event *evt = imalloc(sizeof(struct virtio_input_event));
        while(ring_pop(ring_buf, (u64*)evt) != 0){
            kprint("%c%02x/%02x/%08x\n", evt->type, evt->code, evt->value);
        }
    }

    else if(strcmp(cmd, "whoami") == 0){
        u32 ret = sbi_whoami();
        kprint("I am hart: %u\n", ret);
    }
}
