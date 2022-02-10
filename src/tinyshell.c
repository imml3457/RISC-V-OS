#include <tinyshell.h>
#include <kprint.h>

void tsh(void){
    u8 c;
    char cmd[512];
    u32 cmd_iter = 0;
    kprint("%c> %_");
    while(1){
        while((c = sbi_getchar()) == 0xff){ WFI(); }

        switch(c){
        //handle return keycode
            case '\r':
            case '\n':
                //setting the null character of the command
                cmd[cmd_iter] = '\0';
                kprint("\n");
                exec_cmd(cmd);
                kprint("%c> %_");
                cmd_iter = 0;
                break;

            case 3:
                cmd[0] = '\0';
                cmd_iter = 0;
                kprint("\n%c> %_");
                break;

            case 127:
                if(cmd_iter > 0){
                    kprint("\b \b");
                    cmd_iter--;
                }
            break;
            default:
                if(cmd_iter <= 511){
                    cmd[cmd_iter] = c;
                    cmd_iter++;
                    sbi_putchar(c);
                }
            break;
        }
    }
}
