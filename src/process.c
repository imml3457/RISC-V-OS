#include <process.h>
#include <mmu.h>
#include <page.h>
#include <imalloc.h>
#include <kprint.h>
#include <sbi.h>

struct process* processes[8];

extern u64 spawn_thread_start;
extern u64 spawn_thread_end;
extern u64 spawn_thread_size;

extern u64 spawn_trap_start;
extern u64 spawn_trap_end;
extern u64 spawn_trap_size;

static struct process* create_new_process(struct process* p){

    p->state = PS_UNINITAILIZED;
    p->cntl_block.ptable = page_cont_falloc(1);
    p->cntl_block.stack_pages = page_cont_falloc(1);
    p->cntl_block.number_of_stack_pages = 1;
    p->quantum = PROC_DEFAULT_QUANTUM;
    p->on_hart = -1;


    u64 physcial_addr = virt_to_phys(kernel_page_table, spawn_thread_start);
    mmu_map(p->cntl_block.ptable, spawn_thread_start, physcial_addr, spawn_thread_size, exec);

    physcial_addr = virt_to_phys(kernel_page_table, spawn_trap_start);
    mmu_map(p->cntl_block.ptable, spawn_trap_start, physcial_addr, spawn_trap_size, exec);

    u64 what_frame = (u64)&p->frame;

    physcial_addr = virt_to_phys(kernel_page_table, what_frame);
    mmu_map(p->cntl_block.ptable, what_frame, physcial_addr, sizeof(struct proc_frame), rw);

    p->frame.gpregs[XREG_SP] = DEFAULT_PROCESS_STACK_POINTER + p->cntl_block.number_of_stack_pages * PAGE_SIZE;

    p->frame.sie = SET_SIE_SEIE | SET_SIE_SSIE | SET_SIE_STIE;
    p->frame.sscratch = (u64)&p->frame;
    p->frame.stvec = spawn_trap_start;
    p->frame.trap_satp = SV39 | SET_ASID(OS_ASID) | SET_PPN(kernel_page_table);

    return p;
}


struct process* process_new(){
    struct process* temp = imalloc(sizeof(struct process));
    return create_new_process(temp);
}

struct process* spawn_new_process_user(){
    struct process* p = process_new();
    if(p == NULL){
        return NULL;
    }

    p->os_flag = 0;
    p->frame.sstatus = SSTATUS_FS_INITIAL | SSTATUS_SET_SPIE | SSTATUS_SET_USER;


    u64 stack = (u64)p->cntl_block.stack_pages;
    u64 stack_size = DEFAULT_PROCESS_STACK_POINTER + p->cntl_block.number_of_stack_pages * PAGE_SIZE;

    mmu_map(p->cntl_block.ptable, DEFAULT_PROCESS_STACK_POINTER, stack, stack_size, urw);
    return p;
}
struct process* spawn_new_process_os(){
    struct process* p = process_new();
    if(p == NULL){
        return NULL;
    }

    p->os_flag = 1;
    p->frame.sstatus = SSTATUS_FS_INITIAL | SSTATUS_SET_SPIE | SSTATUS_SET_SUPERVISOR;


    u64 stack = (u64)p->cntl_block.stack_pages;
    u64 stack_size = DEFAULT_PROCESS_STACK_POINTER + p->cntl_block.number_of_stack_pages * PAGE_SIZE;

    mmu_map(p->cntl_block.ptable, DEFAULT_PROCESS_STACK_POINTER, stack, stack_size, rw);
    return p;
}


int spawn_process_on_hart(struct process* p, int hartid){
    u16 hart = sbi_whoami();
    u64 hart_status = sbi_hart_status(hartid);

    if(hart != hartid && hart_status != 1){
        return -1;
    }

    p->on_hart = hartid;
    p->frame.trap_stack = process_stacks[hartid];
    processes[hartid] = p;


    u64 physical_addr = virt_to_phys(p->cntl_block.ptable, p->frame.sscratch);

/*     return 1; */
    return sbi_start_hart(hartid, spawn_thread_start, physical_addr);

}
