#ifndef __TRAP_H__
#define __TRAP_H__

#include <csr.h>
#include <common.h>


//mostly used for OS
struct trapframe{
    s32 gpregs[32];
    s32 fpregs[32];
    u32 pc;
};

#endif
