#ifndef __SBI_H__
#define __SBI_H__

#define SBI_PUTCHAR 0
#define SBI_GETCHAR 1
#define SBI_HART_STATUS 2
#define SBI_SYSTEM_OFF 3
#define SBI_START_HART 4
#define SBI_STOP_HART 5
#define SBI_GET_TIME 6
#define SBI_SET_TIMECMP 7
#define SBI_ADD_TIMECMP 8
#define SBI_ACK_TIMER 9
#define SBI_WHOAMI 10

#include <common.h>

void sbi_putchar(u8 c);
u8 sbi_getchar(void);
u64 sbi_hart_status(u64);
void sbi_system_off(void);
u64 sbi_start_hart(u64 hart, u64 target, u64 privilege);
u64 sbi_stop_hart(void);
u64 sbi_get_time(void);
void sbi_set_timecmp(u64, u64);
void sbi_add_timer(u64, u64);
void sbi_ack_timer(void);
u32 sbi_whoami(void);


#endif
