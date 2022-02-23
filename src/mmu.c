#include <mmu.h>
#include <lock.h>
#include <page.h>

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
                    ((phys_addr + j) >> 12) & 0x1ff,
                    ((phys_addr + j) >> 21) & 0x1ff,
                    ((phys_addr + j) >> 30) & 0x3fffffff
        };
        for(int i = 2; i >= 1; i--){
            ret_entry = p_table->entry[vpn[i]];
            if(!(ret_entry & 1)){
                page_table* tmp = page_cont_falloc(1);
                ret_entry = (((u64)tmp) >> 2) | valid;
                p_table->entry[vpn[i]] = ret_entry;
            }

            p_table = (page_table*)((ret_entry << 2) & ~0xfffUL);
        }

        ret_entry = (ppn[2] << 28) | (ppn[1] << 19) | (ppn[0] << 10) | set_bits | valid;
        p_table->entry[vpn[0]] = ret_entry;
        p_table = p_table_top;
    }
    mutex_unlock(&mmu_lock);
}
