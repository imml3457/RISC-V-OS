#ifndef __TRAP_H__
#define __TRAP_H__

#include <csr.h>
#include <common.h>

#define MAX_IRQ 14
//mostly used for OS
struct trapframe{
    u64 gpregs[32];
    u64 fpregs[32];
    u64 pc;
};

extern struct trapframe SBI_GPREGS[8];

void unhandled_irq(u64, u64);

extern void (*irq_table[])(u64, u64);

#endif
