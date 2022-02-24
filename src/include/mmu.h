#ifndef __MMU_H__
#define __MMU_H__
#include <common.h>
#include <csr.h>

//flush all tlb entries
#define sfence() asm volatile("sfence.vma zero, zero");
//flush all tlb entries whose ASID matches a0
#define sfence_asid(x) asm volatile("sfence.vma zero, %0" : : "r"(x));
//flush tlb entries that match the virtual addr in a0 and asid in a1
#define sfence_asid_both(x, y) asm volatile("sfence.vma %0, %1" : : "r"(y), "r"(x));

#define SV39 (8UL << 60)
#define ASID_BIT 44
#define PPN_BIT 0
#define SET_PPN(pt) ((((u64)pt) >> 12) & 0xFFFFFFFFFFFUL)
#define SET_ASID(bit) ((((u64)bit) & 0xFFFF) << ASID_BIT)
#define OS_ASID 0xFFFFUL

typedef struct ptable{
    u64 entry[512];
}page_table;

typedef enum mmu{
    none = 0,
    valid = 1 << 0,
    read = 1 << 1,
    write = 1 << 2,
    exec = 1 << 3,
    user = 1 << 4,
    global = 1 << 5,
    acc = 1 << 6,
    dirty = 1 << 7,

    valid_dirty = 1 << 0 | 1 << 6 | 1 << 7, //valid bit, accessed bit, and dirty bit all accessed

    //easy money
    rw = 1 << 1 | 1 << 2,
    re = 1 << 1 | 1 << 3,
    rwe = 1 << 1 | 1 << 2 | 1 << 3,

    //more easy money
    //setting user read write execute
    urw = 1 << 1 | 1 << 2 | 1 << 4,
    ure = 1 << 1 | 1 << 3 | 1 << 4,
    urwe = 1 << 1 | 1 << 2 | 1 << 3 | 1 << 4,
}mmu_bits;



void mmu_map(page_table*, u64, u64, u64, u64);
void mmu_unmap(page_table*);
void mmu_map_single(page_table*, u64, u64, u64);
u64  virt_to_phys(page_table*, u64);
void mmu_init();

extern page_table* kernel_page_table;

#endif
