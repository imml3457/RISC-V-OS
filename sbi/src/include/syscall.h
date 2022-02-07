#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#include <common.h>


void syscall_handle(u64 cause, u64 hart);

void unhandled_syscall(u64*);
void syscall_uart_put(u64*);
void syscall_uart_get(u64*);
extern void (*syscall_table[])(u64*);

#endif
