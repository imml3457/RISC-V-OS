#include <page.h>
#include <symbols.h>
#include <lock.h>
#include <utils.h>

page* head;
Mutex pg_lock;
u64 total_num_pages;
u8* top_of_bk;
page* cont_head;

//!!!!!!!!
//need to convert bk bytes from 1 byte each to 2 bits each

void init_cont_page(void){
    total_num_pages = (sym_end(heap) - sym_start(heap)) / PAGE_SIZE;
    top_of_bk = (u8*)sym_start(heap);
    cont_head = (page*)(((sym_start(heap) + total_num_pages) + (PAGE_SIZE - 1)) & 0xfffff000);
    //this should be memsetting 0 for book keeping bytes
    //right now bk is bits each
/*     kprint("top of bk %X\n", top_of_bk + 1); */
    memset(top_of_bk, 0, total_num_pages);
}


page* page_cont_falloc(u64 num_pages){
    u64 i, j;
    u64 found = 0;
    u64 offset = 0;
    for(i = 0; i < total_num_pages; i++){
        if(!((top_of_bk[i] >> 1) & 1)){
            for(j = 1; j < num_pages; j++){
                if((top_of_bk[i+j] >> 1) & 1){
                    break;
                }
                found = 1;
            }
            if(found == 1){
                offset = i;
                break;
            }
        }
    }
    for(i = 0; i < num_pages; i++){
        if(i != num_pages - 1){
            top_of_bk[i + offset] = 2;
        }
        else{
            top_of_bk[i + offset] = 3;
        }
    }

    page* ret_page = cont_head + (PAGE_SIZE * offset);
    kprint("what page offset: %U\n", offset);
    return memset(ret_page, 0, (PAGE_SIZE * num_pages));
}

void page_cont_free(page* pg, u64 num_pages){
    u64 i;
    u64 what_page = (pg - cont_head) / PAGE_SIZE;
    for(i = what_page; i < what_page + num_pages; i++){
        top_of_bk[i] = 0;
    }

    //probably should memset the page when freeing it
}

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
