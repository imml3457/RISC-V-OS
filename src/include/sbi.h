#ifndef __SBI_H__
#define __SBI_H__

#define SBI_PUTCHAR 0
#define SBI_GETCHAR 1
#define SBI_HART_STATUS 2

#include <common.h>

void sbi_putchar(u8 c);
u8 sbi_getchar(void);
u64 sbi_hart_status(u64);

#endif
