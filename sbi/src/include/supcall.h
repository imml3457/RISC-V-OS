#ifndef __SUPCALL_H__
#define __SUPCALL_H__

#include <common.h>


void supcall_handle(u64 cause, u64 hart);

void unhandled_supcall(u64*, u64);
void supcall_uart_put(u64*, u64);
void supcall_uart_get(u64*, u64);
void supcall_hart_status(u64*, u64);
void supcall_system_off(u64*, u64);
void supcall_hart_start(u64*, u64);
void supcall_hart_stop(u64*, u64);
void supcall_get_time(u64*, u64);
void supcall_set_timecmp(u64*, u64);
void supcall_add_timecmp(u64*, u64);
void supcall_ack_timer(u64*, u64);
void supcall_whoami(u64*, u64);
extern void (*supcall_table[])(u64*, u64);

#endif
