#ifndef __C_HELPERS_H__
#define __C_HELPERS_H__

#include <common.h>

int strcmp(char*, char*);

int strcpy(char*, char*);

void exec_cmd(char*);

unix_time get_unix_time(u64);

#endif
