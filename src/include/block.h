#ifndef __BLOCK_H__
#define __BLOCK_H__

#include <common.h>

void dsk_read(void*, u64, u64);
void dsk_write(void*, u64, u64);

#endif
