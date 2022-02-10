#include <kprint.h>
#include <common.h>
#include <sbi.h>
#include <c_helpers.h>


int main(u64 hart){

    kprint_set_putc(sbi_putchar);
    kprint_set_getc(sbi_getchar);
    u8 c;
    char cmd[512];
    u32 cmd_iter = 0;
    kprint("%c> %_");
    while(1){
        while((c = sbi_getchar()) == 0xff){ WFI(); }
        //handle return keycode
        if(c == '\r' || c == '\n'){
            //setting the null character of the command
            cmd[cmd_iter] = '\0';
            kprint("\n");
            exec_cmd(cmd);
            kprint("%c> %_");
            cmd_iter = 0;
        }

        else if(c == '\b' || c == 127){
            if(cmd_iter > 0){
                kprint("\b \b");
                cmd_iter--;
            }
        }

        else{
            if(cmd_iter <= 511){
                cmd[cmd_iter] = c;
                cmd_iter++;
                sbi_putchar(c);
            }
        }
    }
    return 0;
}
