#include <page.h>
#include <symbols.h>
#include <lock.h>
#include <utils.h>

page* head;
Mutex pg_lock;
u64 total_num_pages;
u8* top_of_bk;
page* cont_head;
u64 num_of_bk;

void init_cont_page(void){
    //getting some globals for the book keeping
    //also memsetting the book keeping bytes
    total_num_pages = (sym_end(heap) - sym_start(heap)) / PAGE_SIZE;
    top_of_bk = (u8*)sym_start(heap);
    num_of_bk = total_num_pages / 4;
    cont_head = (page*)(((sym_start(heap) + num_of_bk) + (PAGE_SIZE - 1)) & 0xfffff000);
    memset(top_of_bk, 0, num_of_bk);
}


page* page_cont_falloc(u64 num_pages){
    u64 i;
    u64 found = 0;
    u64 offset = 0;
    //finding if there is an x amount of pages free
    for(i = 0; i < 4 * num_of_bk; i++){
        u64 two_bit = top_of_bk[i / 4] >> GET_INDEX(i);
        if(!(two_bit & 2)){
            found += 1;
        }
        else{
            found = 0;
        }
        if(found == num_pages){
            offset = i - (num_pages - 1);
            break;
        }

    }

    if(found != num_pages){
        goto fail;
    }

    //actually setting the book keeping
    for(i = 0; i < num_pages; i++){
        u64 indx = offset + i;
        u64 set_bit;
        if(i != num_pages - 1){
            set_bit = 0b10;
            set_bit = set_bit << GET_INDEX(indx);
        }
        else{
            set_bit = 0b11;
            set_bit = set_bit << GET_INDEX(indx);
        }
        top_of_bk[(indx) / 4] |= set_bit;
    }

    page* ret_page = (page*)((u64)cont_head + (PAGE_SIZE * offset));
    return memset(ret_page, 0, (PAGE_SIZE * num_pages));

fail:
    return NULL;
}

void page_cont_free(page* pg){
    u64 i = 0;
    u64 what_page = (pg - cont_head) / PAGE_SIZE;
    u64 two_bit;
    do{
        //freeing the bookkeeping bytes
        two_bit = top_of_bk[(what_page + i) / 4] >> GET_INDEX(what_page + i);
        kprint("what is the get index %U\n", GET_INDEX(what_page + i));
        switch(GET_INDEX(what_page + i)){
            case 6:
                top_of_bk[(what_page + i) / 4] &= 0b00111111;
                break;
            case 4:
                top_of_bk[(what_page + i) / 4] &= 0b11001111;
                break;
            case 2:
                top_of_bk[(what_page + i) / 4] &= 0b11110011;
                break;
            case 0:
                top_of_bk[(what_page + i) / 4] &= 0b11111100;
                break;
        }
        i++;
    }while(two_bit != 3);
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
