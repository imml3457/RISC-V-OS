#ifndef __SUPCALL_H__
#define __SUPCALL_H__

#include <common.h>


void supcall_handle(u64 cause, u64 hart);

void unhandled_supcall(u64*);
void supcall_uart_put(u64*);
void supcall_uart_get(u64*);
void supcall_hart_status(u64*);
extern void (*supcall_table[])(u64*);

#endif
