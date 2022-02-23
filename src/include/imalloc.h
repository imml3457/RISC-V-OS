#ifndef __IMALLOC_H__
#define __IMALLOC_H__

#include <utils.h>
#include <common.h>
#include <stddef.h>

typedef struct Freelist{
    size_t size;
    struct Freelist *next;
    struct Freelist *prev;
}Flist;

void* imalloc(size_t);
void imfree(void*);
void imalloc_init();
void coalesce_free_list();
void print_flist();

#endif
