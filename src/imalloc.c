#include <imalloc.h>
#include <page.h>
#include <kprint.h>

Flist* imalloc_head;
u64 total_num_pages;

void append_node(Flist* first, Flist* second){
    //using firsts next to be the new nodes next
    //then the new nodes prev will be first
    second->next = first->next;
    second->prev = first;
    //since firsts->next's node (before the move) (and prev)
    //is now wrong we need to set it the new node
    second->next->prev = second;
    second->prev->next = second;
}

void remove_node(Flist* first){
    //setting the nodes previous and next
    //to the approriate values to essentially remove
    //the node from the list
    first->next->prev = first->prev;
    first->prev->next = first->next;
}

void imalloc_init(void){
    Flist* ptr = page_cont_falloc(1);
    total_num_pages = 0;
    imalloc_head = ptr;
    imalloc_head->size = 0;
    //needs to be circular
    imalloc_head->next = imalloc_head->prev = imalloc_head;
}

void* imalloc(size_t num_bytes){
    Flist* tmp;
    Flist* ptr;
    u64 tmp_num_pages = 0;
    //getting this from my malloc
    num_bytes = (num_bytes + sizeof(size_t) + sizeof(Flist)) & -8UL;

    for(tmp = imalloc_head->next; tmp != imalloc_head && tmp->size < num_bytes; tmp = tmp->next);

    //still at top of head
    if(tmp == imalloc_head){
        u64 num_pages = (num_bytes / PAGE_SIZE) + 1;
        if(total_num_pages > 1){
            total_num_pages += 1;
            tmp_num_pages = (num_pages - 1);
        }
        else{
            total_num_pages += 1;
        }
/*         kprint("total num pages %U and num_pages allocated %U\n", total_num_pages, num_pages); */
/*         kprint("tmp size and addr %D %X\n", tmp->size, tmp); */
/*         tmp = (Flist*)(((total_num_pages) * PAGE_SIZE) + (u64)tmp); */
/*         kprint("tmp size and addr %D %X\n", tmp->size, tmp); */
        ptr = page_cont_falloc(num_pages);
        tmp = (Flist*)((u64) ptr);
        if(ptr == NULL){
            return NULL;
        }

        ptr->size = num_pages * (PAGE_SIZE - sizeof(size_t));
        total_num_pages += tmp_num_pages;
        append_node(imalloc_head->prev, ptr);
        coalesce_free_list();
    }
    char* ret = (char*)tmp + tmp->size - num_bytes;
    if(tmp->size - num_bytes < 8){
        remove_node(tmp);
    }
    else{
        tmp->size -= *(size_t *)ret = num_bytes;
    }
/*     kprint("malloc addr and size: %X %U\n", ret + sizeof(size_t), ((Flist*)ret)->size); */
    return ret + sizeof(size_t);

}

void imfree(void* ptr){
    Flist* tmp;
    ptr -= sizeof(size_t);
    for(tmp = imalloc_head->prev; tmp != imalloc_head && ptr < (void*)tmp; tmp = tmp->prev);
    append_node(tmp, ptr);
    coalesce_free_list();
}

void coalesce_free_list(){
    Flist* tmp = imalloc_head->next;
    while(tmp->next != imalloc_head){
        if((char*)tmp + tmp->size == (char*)tmp->next){
            tmp->size += tmp->next->size;
            remove_node(tmp->next);
        }
        else{
            tmp = tmp->next;
        }
    }
}

void print_flist(){
    Flist* tmp;
    kprint("imalloc_head addr and size: %X %D\n", imalloc_head, imalloc_head->size);
    for(tmp = imalloc_head->next; tmp != imalloc_head; tmp = tmp->next){
        kprint("Free list addr %X and size %D\n", tmp, tmp->size);
    }
}
