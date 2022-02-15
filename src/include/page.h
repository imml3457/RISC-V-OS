#ifndef __PAGE_H__
#define __PAGE_H__
#include <common.h>

#define PAGE_SIZE 4096

typedef struct Page{
    struct Page* next;
}page;


void initialize_page(void);
page* page_falloc(void);
void page_free(page*);

#endif
