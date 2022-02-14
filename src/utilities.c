#include <utilities.h>
#include <kprint.h>
#include <common.h>
#include <csr.h>

#define IS_LEAP(x) ((x) & 0x3)

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

    return u_time;
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
        volatile u32* RTC_BASE = (void*)0x101000UL;
        volatile u32* RTC_STATUS = ((void*)RTC_BASE) + 0x04;
        u64 low = (*RTC_BASE);
        u64 high = (*RTC_STATUS);

        u64 time = (high << 32) | low;
        unix_time u_time;

        u_time = get_unix_time(time);

        kprint("YEAR: %U\n", u_time.year);
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
        start_hart(hart);

    }

}
