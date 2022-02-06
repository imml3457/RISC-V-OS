#ifndef __TRAP_H__
#define __TRAP_H__

#include <csr.h>
#include <common.h>

//mostly used for OS
struct trapframe{
    u64 gpregs[32];
    u64 fpregs[32];
    u64 pc;
};

#endif
