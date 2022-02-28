#include <mmu.h>
#include <lock.h>
#include <page.h>
#include <symbols.h>

page_table* kernel_page_table;
Mutex mmu_lock;

void mmu_map(page_table* p_table, u64 virt_addr, u64 phys_addr, u64 addr_range, u64 set_bits){
    page_table* p_table_top = p_table;
    u64 ret_entry = 0;
     //starting at the highest entry of a page table
     //there are 3 levels
     //gb, mb, kb

    mutex_spinlock(&mmu_lock);
    for(u64 j = 0; j < addr_range; j = PAGE_SIZE + j){
        u64 vpn[] = {
                    //this is spliting the vpn check spec if you forget
                    ((virt_addr + j) >> 12) & 0x1ff,
                    ((virt_addr + j) >> 21) & 0x1ff,
                    ((virt_addr + j) >> 30) & 0x1ff
        };

        u64 ppn[] = {
                    //likewise for ppn
                    ((phys_addr + j) >> 12) & 0x1ff,
                    ((phys_addr + j) >> 21) & 0x1ff,
                    ((phys_addr + j) >> 30) & 0x3ffffff
        };

        //start at the top of level of page table
        for(int i = 2; i >= 1; i--){
            ret_entry = p_table->entry[vpn[i]];
            //if it is invalid
            if(!(ret_entry & valid)){

                //give it a page
                page_table* tmp = page_cont_falloc(1);
                //set the bit to valid
                ret_entry = (((u64)tmp) >> 2) | valid;
                p_table->entry[vpn[i]] = ret_entry;
            }
            //setting up to move down a level
            p_table = (page_table*)((ret_entry << 2) & ~0xfffUL);
        }
        //setting the bottom as a leaf
        ret_entry = (ppn[2] << 28) | (ppn[1] << 19) | (ppn[0] << 10) | set_bits | valid;
        p_table->entry[vpn[0]] = ret_entry;
        //need this to do multiple mappings
        p_table = p_table_top;
    }
    mutex_unlock(&mmu_lock);
}
void mmu_map_single(page_table* p_table, u64 virt_addr, u64 phys_addr, u64 set_bits){
    u64 ret_entry;
    u64 vpn[] = {
                //this is spliting the vpn check spec if you forget
                (virt_addr >> 12) & 0x1ff,
                (virt_addr >> 21) & 0x1ff,
                (virt_addr >> 30) & 0x1ff
    };

    u64 ppn[] = {
                (phys_addr >> 12) & 0x1ff,
                (phys_addr >> 21) & 0x1ff,
                (phys_addr >> 30) & 0x3ffffff
    };
     //starting at the highest entry of a page table
     //there are 3 levels
     //gb, mb, kb

    mutex_spinlock(&mmu_lock);
    for(int i = 2; i >= 1; i--){
        ret_entry = p_table->entry[vpn[i]];
        if(!(ret_entry & valid)){
            page_table* tmp = page_cont_falloc(1);
            ret_entry = (((u64)tmp) >> 2) | valid;
            p_table->entry[vpn[i]] = ret_entry;
        }

        p_table = (page_table*)((ret_entry << 2) & ~0xfffUL);
    }

    ret_entry = (ppn[2] << 28) | (ppn[1] << 19) | (ppn[0] << 10) | set_bits | valid;
    p_table->entry[vpn[0]] = ret_entry;
    mutex_unlock(&mmu_lock);
}

void mmu_unmap(page_table* p_table){
    u64 table_entry = 0;
    for(int i = 0; i < 512; i++){
        table_entry = p_table->entry[i];
        //checking if valid
        if(table_entry & valid){
            //checking if it is a branch
            if(!(table_entry & rwe)){
                //if it is move down a level
                //and recurse
                table_entry = (table_entry << 2) & ~0xfffUL;
                mmu_unmap((page_table*) table_entry);
            }
            else{
                //if it isa leaf just set it to 0
                p_table->entry[i] = 0;
            }
        }
    }

    page_cont_free(p_table);
}

u64 virt_to_phys(page_table* p_table, u64 virt_addr){
    u64 p_table_entry = 0;
    u64 vpn[] = {
                //this is spliting the vpn check spec if you forget
                (virt_addr >> 12) & 0x1ff,
                (virt_addr >> 21) & 0x1ff,
                (virt_addr >> 30) & 0x1ff
    };


    for(int i = 2; i >= 0; i--){
        //get a vpn entry
        p_table_entry = p_table->entry[vpn[i]];
        //if it is invalid fail
        if(!(p_table_entry & valid)){
            return -1;
        }
        //if the rwe is set we are a leaf
        else if(p_table_entry & rwe){
            u64 ppn[] = {
                        (p_table_entry >> 10) & 0x1ff,
                        (p_table_entry >> 19) & 0x1ff,
                        (p_table_entry >> 28) & 0x3ffffff
            };
            //getting the different offsets dependent on level
            if(i == 0){
                return (ppn[2] << 30) | (ppn[1] << 21) | (ppn[0] << 12) | (virt_addr & 0xfffUL);
            }
            else if(i == 1){
                return (ppn[2] << 30) | (ppn[1] << 21) | (virt_addr & 0xfffffUL);
            }
            else if(i == 2){
                return (ppn[2] << 30) | (virt_addr & 0x3fffffffUL);
            }
            else{
                return -1;
            }

        }
        else{
            //moving on down
            p_table = (page_table*)((p_table_entry << 2) & ~0xfffUL);
        }
    }

    return -1;
}

void mmu_init(){
    mmu_map(kernel_page_table, sym_start(text), sym_start(text), sym_size(text), re);
    mmu_map(kernel_page_table, sym_start(data), sym_start(data), sym_size(data), rw);
    mmu_map(kernel_page_table, sym_start(rodata), sym_start(rodata), sym_size(rodata), read);
    mmu_map(kernel_page_table, sym_start(bss), sym_start(bss), sym_size(bss), rw);
    mmu_map(kernel_page_table, sym_start(stack), sym_start(stack), sym_size(stack), rw);
    mmu_map(kernel_page_table, sym_start(heap), sym_start(heap), sym_size(heap), rw);

    mmu_map(kernel_page_table, 0x0c000000, 0x0c000000, 0x0c2fffff - 0x0c000000, rw);

    CSR_WRITE("satp", SV39 | SET_ASID(OS_ASID) | SET_PPN(kernel_page_table));
    sfence();
}
