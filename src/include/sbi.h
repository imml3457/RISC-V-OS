#ifndef __SBI_H__
#define __SBI_H__

#define SBI_PUTCHAR 0
#define SBI_GETCHAR 1

#include <common.h>

void sbi_putchar(u8 c);
u8 sbi_getchar(void);


#endif
