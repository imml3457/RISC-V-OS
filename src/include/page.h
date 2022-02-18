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
page* page_falloc(void);
void page_free(page*);

void init_cont_page(void);
page* page_cont_falloc(u64);
void page_cont_free(page*);

#endif
