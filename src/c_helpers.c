#include <c_helpers.h>
#include <kprint.h>

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


void exec_cmd(char* cmd){
    if(strcmp(cmd, "test") == 0){
        kprint("this is a test!\n");
    }
    if(strcmp(cmd, "gettime()") == 0){
        u64 tm;
        asm volatile("rdtime %0" : "=r"(tm));
        kprint("%U\n", tm);
    }
}
