#include <c_helpers.h>
#include <kprint.h>

#define IS_LEAP(x) ((x) & 0x3)

int strcmp(char* str1, char* str2){
    //error checking the strings
    while(*str1 != '\0' && *str2 != '\0'){
        if(*str1 != *str2){
            //return 1 if they is discrepancy
            return 1;
        }
        str1++;
        str2++;
    }

    if(!(*str1 | *str2)){
        return 0;
    }

    //in the future
    //compare which str is the \0
    //so you can get the "length"
    //check compliance
    return -1;
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

void exec_cmd(char* cmd){

    //for executing command
    if(strcmp(cmd, "test") == 0){
        kprint("this is a test!\n");
    }
    if(strcmp(cmd, "gettime()") == 0){
        u64 tm;
        asm volatile("rdtime %0" : "=r"(tm));
        kprint("%U\n", tm);
    }
    if(strcmp(cmd, "gettimeofday()") == 0){
        volatile u32* RTC_BASE = (void*)0x101000UL;
        volatile u32* RTC_STATUS = ((void*)RTC_BASE) + 0x04;
        u64 low = (*RTC_BASE);
        u64 high = (*RTC_STATUS);

        u64 time = (high << 32) | low;
        unix_time u_time;

        u_time = get_unix_time(time);

        kprint("YEAR: %U\n", u_time.year);
    }
    if(strcmp(cmd, "status()") == 0){
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


}
