#include <utils.h>
#include <kprint.h>
#include <common.h>
#include <csr.h>
#include <page.h>

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

u64 get_nano_time(){
    volatile u32* RTC_BASE = (void*)0x101000UL;
    volatile u32* RTC_STATUS = ((void*)RTC_BASE) + 0x04;
    u64 low = (*RTC_BASE);
    u64 high = (*RTC_STATUS);
    u64 time = (high << 32) | low;
    return time;
}

unix_time get_unix_time(u64 time){
    unix_time u_time;
    u64 year_time;
    u64 running_year = 0;
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
    //hard code eastern time
    u_time.hour -= 5;
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

void start_hart(u64 hart){
    u64 status = sbi_hart_status(hart);
    if(status == 0){
        kprint("Hart %U does not exist\n", hart);
    }
    else if(status != 1){
        kprint("Hart %U is not stopped\n",hart);
    }
    else{
        sbi_start_hart(hart, test_hart, 1);
    }
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
    else if(strncmp(cmd, "start", 5) == 0){
        // I should error check
        // out of bounds hart
        // and get spacing better
        u64 hart = atoi(cmd + 6);
        if(hart > 9){
            kprint("hart is huge please stop %U\n", hart);
        }
        else{
            start_hart(hart);
        }
    }
    else if(strcmp(cmd, "allocate_page") == 0){
        page* pg = page_falloc();
        page* pg2 = page_falloc();
        page* pg3 = page_falloc();
        kprint("page addr %X: %X: %X\n", pg, pg2, pg3);
        page_free(pg);
        page_free(pg2);
        page_free(pg3);
    }
}