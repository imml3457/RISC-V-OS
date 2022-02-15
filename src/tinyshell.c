#include <tinyshell.h>
#include <kprint.h>

void print_prompt(){
    kprint("%g");
    print_unix_time();
    kprint("%_ %c> %_");
}
void print_prompt_failure(){
    kprint("\n%g");
    print_unix_time();
    kprint("%_ %r> %_");
}

void tsh(void){
    u8 c;
    u8 esc_f, esc_s;
    u8 tmp;
    char cmd[512];
    char prev[512];
    u32 cmd_iter = 0;
    cmd[0]  = 0;
    prev[0] = 0;
    u32 cmd_prev_iter = 0;
    print_prompt();
    while(1){
        while((c = sbi_getchar()) == 0xff){ WFI(); }

        switch(c){
        //handle return keycode
            case '\r':
            case '\n':
                //setting the null character of the command
                strcpy(prev, cmd);
                cmd_prev_iter = cmd_iter;
                cmd[cmd_iter] = '\0';
                kprint("\n");
                exec_cmd(cmd);
                print_prompt();
                cmd_iter = 0;
                break;

            case 3:
                cmd[0] = '\0';
                cmd_iter = 0;
                print_prompt_failure();
/*                 kprint("\n%r> %_"); */
                break;

            case 127:
                if(cmd_iter > 0){
                    kprint("\b \b");
                    cmd_iter--;
                }
                break;
            case 27:
                esc_f = sbi_getchar();
                if(esc_f == '['){
                    esc_s = sbi_getchar();
                    switch(esc_s){
                        case 'A':
                            strcpy(cmd, prev);
                            cmd_iter = cmd_prev_iter;
                            u32 i;
                            for(i = 0; i < cmd_iter; i++){
                                sbi_putchar(cmd[i]);
                            }
                            break;
                    }
                }
                break;
            case 12:
                kprint("\e[2J");
                kprint("\e[H");
                print_prompt();
                int i;
                for(i = 0; i < cmd_iter; i++){
                    sbi_putchar(cmd[i]);
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
