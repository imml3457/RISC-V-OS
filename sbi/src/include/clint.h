#ifndef __CLINT_H__
#define __CLINT_H__
#include <common.h>


void clint_set_msip(u64);
void clint_clear_msip(u64);
void clint_set_mtimecmp(u64, u64);
u64 clint_get_time(void);

#endif

