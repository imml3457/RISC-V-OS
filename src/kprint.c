#include "kprint.h"

static void (*_kprint_putc)(u8);
static u8 (*_kprint_getc)(void);

static void _kputs(const char *s) {
    if (_kprint_putc == NULL) { return; }

    while (*s) { _kprint_putc(*s); s += 1; }
}

static char * dtos(char *p, s32 d) {
    int neg;

    neg = d < 0;

    if (neg) { d = -d; }

    p += 3 * sizeof(s32);
    *--p = 0;

    do {
        *--p = '0' + d % 10;
        d /= 10;
    } while (d);

    if (neg) { *--p = '-'; }

    return p;
}

static char * Dtos(char *p, s64 d) {
    int neg;

    neg = d < 0;

    if (neg) { d = -d; }

    p += 3 * sizeof(s64);
    *--p = 0;

    do {
        *--p = '0' + d % 10;
        d /= 10;
    } while (d);

    if (neg) { *--p = '-'; }

    return p;
}

static char * utos(char *p, u32 d) {
    p += 3 * sizeof(u32);
    *--p = 0;

    do {
        *--p = '0' + d % 10;
        d /= 10;
    } while (d);

    return p;
}

static char * Utos(char *p, u64 d) {
    p += 3 * sizeof(u64);
    *--p = 0;

    do {
        *--p = '0' + d % 10;
        d /= 10;
    } while (d);

    return p;
}

static char * xtos(char *p, u32 x) {
    const char *digits = "0123456789ABCDEF";

    p += 3 * sizeof(u32);
    *--p = 0;

    while (x) {
        *--p = digits[x & 0xf];
        x >>= 4;
    }

    return p;
}

static char * Xtos(char *p, u64 x) {
    const char *digits = "0123456789ABCDEF";

    p += 3 * sizeof(u64);
    *--p = 0;

    while (x) {
        *--p = digits[x & 0xf];
        x >>= 4;
    }

    return p;
}

static const char * eat_pad(const char *s, int *pad) {
    int neg;

    if (!*s) { return s; }

    *pad = 0;

    neg = *s == '-';

    if (neg) { s += 1; }

    while (*s >= '0' && *s <= '9') {
        *pad = 10 * *pad + (*s - '0');
        s += 1;
    }

    if (neg) { *pad = -*pad; }

    return s;
}

void vkprint(const char *fmt, va_list args) {
    int   last_was_perc;
    char  c;
    int   pad;
    char  buff[64];
    char *p;
    int   p_len;

    if (_kprint_putc == NULL) { return; }

    last_was_perc = 0;

    while ((c = *fmt)) {
        if (c == '%' && !last_was_perc) {
            last_was_perc = 1;
            goto next;
        }

        if (last_was_perc) {
            pad = 0;

            if (c == '-' || (c >= '1' && c <= '9')) {
                fmt = eat_pad(fmt, &pad);
                c = *fmt;
            }

            switch (c) {
                case '%': p = "%";                           break;
                case '_': p = "\e[0m";                       break;
                case 'k': p = "\e[30m";                      break;
                case 'b': p = "\e[34m";                      break;
                case 'g': p = "\e[32m";                      break;
                case 'c': p = "\e[36m";                      break;
                case 'r': p = "\e[31m";                      break;
                case 'y': p = "\e[33m";                      break;
                case 'K': p = "\e[40m";                      break;
                case 'B': p = "\e[44m";                      break;
                case 'G': p = "\e[42m";                      break;
                case 'C': p = "\e[46m";                      break;
                case 'R': p = "\e[41m";                      break;
                case 'Y': p = "\e[43m";                      break;
                case 'd': p = dtos(buff, va_arg(args, s32)); break;
                case 'D': p = Dtos(buff, va_arg(args, s64)); break;
                case 'u': p = utos(buff, va_arg(args, u32)); break;
                case 'U': p = Utos(buff, va_arg(args, u64)); break;
                case 'x': p = xtos(buff, va_arg(args, u32)); break;
                case 'X': p = Xtos(buff, va_arg(args, u64)); break;
                case 's': p = va_arg(args, char*);           break;

                default: goto noprint;
            }

            for (p_len = 0; p[p_len]; p_len += 1);

            for (; pad - p_len > 0; pad -= 1) { _kprint_putc(' '); }

            _kputs(p);

            for (; pad + p_len < 0; pad += 1) { _kprint_putc(' '); }

noprint:;
            last_was_perc = 0;
        } else {
            _kprint_putc(*fmt);
        }

next:;
        fmt += 1;
    }
}

void kprint(const char *fmt, ...) {
    va_list args;

    va_start(args, fmt);
    vkprint(fmt, args);
    va_end(args);
}

void kputc(u8 c) {
    if (_kprint_putc == NULL) { return; }

    _kprint_putc(c);
}

u8 kgetc(void) {
    if (_kprint_getc == NULL) { return 0xff; }

    return _kprint_getc();
}

void kprint_set_putc(void (*putc)(u8)) { _kprint_putc = putc; }
void kprint_set_getc(u8 (*getc)(void)) { _kprint_getc = getc; }
