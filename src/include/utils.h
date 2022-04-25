#ifndef __UTILS_H__
#define __UTILS_H__

#include <common.h>
/* #include <imalloc.h> */

int strcmp(char*, char*);

int strncmp(char*, char*, u32);

int strcpy(char*, char*);

int strlen(const char*);

void exec_cmd(char*);

void test_hart(void);

u64 get_nano_time(void);

unix_time get_unix_time(u64);

int atoi(const char*);

void* memset(void*, u32, u64);

void* memcpy(void*, void*, u64);

void memmove(void*, void*, u64);

void print_unix_time(void);

void hexdump(const u8 *bytes, u32 n_bytes);

/* Flist* insertionSort(Flist*); */

#endif
