#ifndef __KPRINT_H__
#define __KPRINT_H__

#include "common.h"

#include <stdarg.h>

void kputc(u8 c);
u8   kgetc(void);
void vkprint(const char *fmt, va_list args);
void kprint(const char *fmt, ...);
void kprint_set_putc(void (*putc)(u8));
void kprint_set_getc(u8 (*getc)(void));

#endif
