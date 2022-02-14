#ifndef __UTILITIES_H__
#define __UTILITIES_H__

#include <common.h>

int strcmp(char*, char*);

int strncmp(char*, char*, u32);

int strcpy(char*, char*);

void exec_cmd(char*);

void test_hart(void);

u64 get_nano_time(void);

unix_time get_unix_time(u64);

int atoi(const char*);

#endif
