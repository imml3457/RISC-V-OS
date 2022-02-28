#include <imalloc.h>
#include <page.h>
#include <kprint.h>
#include <mmu.h>

Flist* imalloc_head;
u64 total_num_pages;

u64 starting_virt_addr =  0x130000000UL;

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
    //mapping the sentinel node
    mmu_map_single(kernel_page_table, starting_virt_addr, (u64)ptr, rw);
    sfence_asid(0xffffUL);
/*     total_num_pages = 0; */
    imalloc_head = (Flist*)starting_virt_addr;
    imalloc_head->size = 0;
    //needs to be circular
    imalloc_head->next = imalloc_head->prev = imalloc_head;
}

void* imalloc(size_t num_bytes){
    Flist* tmp;
    Flist* ptr, *virt;
    u64 tmp_num_pages = 0;
    //getting this from my malloc
    num_bytes = (num_bytes + sizeof(size_t) + sizeof(Flist)) & -16UL;
    //getting to a chunk with enough free space or the end
    for(tmp = imalloc_head->next; tmp != imalloc_head && tmp->size < num_bytes; tmp = tmp->next);

    //still at top of head
    if(tmp == imalloc_head){
        //if we could not find a chunk with enough size
        //we need to give a page out

        //minimum page size is 8192
        u64 num_pages = (num_bytes / PAGE_SIZE) + 2;
/*         if(total_num_pages > 1){ */
/*             total_num_pages += 1; */
/*             tmp_num_pages = (num_pages - 1); */
/*         } */
/*         else{ */
/*             total_num_pages += 1; */
/*         } */

        //update the starting address
        starting_virt_addr += (PAGE_SIZE * (num_pages));
        ptr = page_cont_falloc(num_pages);
        if(ptr == NULL){
            return NULL;
        }
        virt = (Flist*)starting_virt_addr;
/*         kprint("0x%X\n", starting_virt_addr); */
        tmp = (Flist*)(starting_virt_addr);
        mmu_map(kernel_page_table, starting_virt_addr, (u64)ptr, (PAGE_SIZE * num_pages), rw);
        sfence_asid(0xffffUL);
/*         tmp = (Flist*)((u64) ptr); */

        virt->size = num_pages * (PAGE_SIZE - sizeof(size_t));
        total_num_pages += tmp_num_pages;
        append_node(imalloc_head->prev, virt);
        coalesce_free_list();
    }
    char* ret = (char*)tmp + tmp->size - num_bytes;
    if(tmp->size - num_bytes < 16){
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
    //getting the size of the block minus the bookkeeping
    ptr = ptr - sizeof(size_t);
    //appending the free node
    for(tmp = imalloc_head->prev; tmp != imalloc_head && ptr < (void*)tmp; tmp = tmp->prev);
    append_node(tmp, ptr);
    coalesce_free_list();
}

void coalesce_free_list(){
    Flist* tmp;
    //looping throught the free list to find what
    //blocks can be coalesced
    for(tmp = imalloc_head->next; tmp->next != imalloc_head; tmp = tmp->next){
        if((char*)tmp + tmp->size == (char*)tmp->next){
            tmp->size += tmp->next->size;
            remove_node(tmp->next);
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
