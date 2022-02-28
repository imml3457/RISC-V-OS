#ifndef __PAGE_H__
#define __PAGE_H__
#include <common.h>
#include <kprint.h>

#define PAGE_SIZE 4096
#define GET_INDEX(x) ( ( 2 * (4 - (x % 4) ) ) - 2)

typedef struct Page{
    struct Page* next;
}page;


void initialize_page(void);
void* page_falloc(void);
void page_free(void*);

void init_cont_page(void);
void* page_cont_falloc(u64);
void page_cont_free(void*);

extern page* cont_head;

#endif
