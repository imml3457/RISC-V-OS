#include <page.h>
#include <symbols.h>
#include <lock.h>
#include <utils.h>

page* head;
Mutex pg_lock;

void page_init(){
    //getting the top address of the heap
    head = (page*)sym_start(heap);

    page* pg = head;
    page* next = pg;

    while((u64)next < sym_end(heap)){
        //setting the free list
        pg->next = next;
        pg = next;
        next = (page*)(pg + PAGE_SIZE);
    }
    //the end of the free list
    //essentially when you are out of pages
    pg->next = NULL;
}

//f stands for something rather
page* page_falloc(){
    //if you are out of space on heap
    if(head == NULL){
        return NULL;
    }
    mutex_spinlock(&pg_lock);
    page* page_return = head;
    head = head->next;
    mutex_unlock(&pg_lock);
    //return the pointer to the page
    return memset(page_return, 0, PAGE_SIZE);
}

void page_free(page* pg){
    mutex_spinlock(&pg_lock);
    pg->next = head;
    head = pg;
    mutex_unlock(&pg_lock);
}
