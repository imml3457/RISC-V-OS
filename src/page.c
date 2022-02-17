#include <page.h>
#include <symbols.h>
#include <lock.h>
#include <utils.h>

page* head;
Mutex pg_lock;

void initialize_page(){
    //getting the top address of the heap
    head = (page*)sym_start(heap);

    page* pg = head;
    page* next = pg;

    while((u64)next < sym_end(heap)){
        //setting the free list

        //this is setting original pages next
        pg->next = next;
        //setting pg for the next iteration
        pg = next;
        //moving to the next pages address i.e. incementing next
        next = (page*)(((u64)pg) + PAGE_SIZE);
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
    //simply take what is on the top of the list
    page* page_return = head;
    //make the head equal what is next on the list
    head = head->next;
    mutex_unlock(&pg_lock);
    //return the pointer to the page
    return memset(page_return, 0, PAGE_SIZE);
}

void page_free(page* pg){
    mutex_spinlock(&pg_lock);
    //adding the page below the head
    pg->next = head;
    head = pg;
    mutex_unlock(&pg_lock);
}
