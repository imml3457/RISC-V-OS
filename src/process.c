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


    //setting the state of the process
    p->state = PS_NONE;

    //give space for the page table and stack
    p->cntl_block.ptable = page_cont_falloc(1);
    p->cntl_block.stack_pages = page_cont_falloc(1);

    //start off with 1 stack page
    p->cntl_block.number_of_stack_pages = 1;

    //setting quantum and what hart it is on
    //default to -1 since it is not on a hart
    p->quantum = PROC_DEFAULT_QUANTUM;
    p->on_hart = -1;

    //mapping the thread start into the process page table
    u64 physcial_addr = virt_to_phys(kernel_page_table, spawn_thread_start);
    mmu_map(p->cntl_block.ptable, spawn_thread_start, physcial_addr, spawn_thread_size, exec);
    //map the trap start into the process page table
    physcial_addr = virt_to_phys(kernel_page_table, spawn_trap_start);
    mmu_map(p->cntl_block.ptable, spawn_trap_start, physcial_addr, spawn_trap_size, exec);

    //get the trap frame
    u64 what_frame = (u64)&p->frame;

    //find the trap frame in page table and then map it into the process pt
    physcial_addr = virt_to_phys(kernel_page_table, what_frame);
    mmu_map(p->cntl_block.ptable, what_frame, physcial_addr, sizeof(struct proc_frame), rw);


    //setting stack pointer
    p->frame.gpregs[XREG_SP] = DEFAULT_PROCESS_STACK_POINTER + p->cntl_block.number_of_stack_pages * PAGE_SIZE;


    //setting up the process registers
    p->frame.sie = SET_SIE_SEIE | SET_SIE_SSIE | SET_SIE_STIE;
    p->frame.sscratch = (u64)&p->frame;
    p->frame.stvec = spawn_trap_start;

    //satp stuff
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
    //not the os
    p->os_flag = 0;

    //set user! important
    p->frame.sstatus = SSTATUS_FS_INITIAL | SSTATUS_SET_SPIE | SSTATUS_SET_USER;

    //getting the stack address and size
    u64 stack = (u64)p->cntl_block.stack_pages;
    u64 stack_size = DEFAULT_PROCESS_STACK_POINTER + p->cntl_block.number_of_stack_pages * PAGE_SIZE;
    //those were for mapping the stack into the page table
    mmu_map(p->cntl_block.ptable, DEFAULT_PROCESS_STACK_POINTER, stack, stack_size, urw);
    return p;
}
struct process* spawn_new_process_os(){

    //similar to the user
    //but os is 1
    //and set supervisor (and permission stuff)
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

    //making sure the hart is STOPPED and the hart we wanna spawn on is not ourselves
    if(hart != hartid && hart_status != 1){
        kprint("here in process start\n");
        return -1;
    }

    //setting the hart id for the process
    p->on_hart = hartid;

    //setting the trap_stack from the global (1 page initialized)
    p->frame.trap_stack = process_stacks[hartid];

    //what hart has the current process
    //currently unused
/*     processes[hartid] = p; */


/*     sbi_add_timer(hartid, p->quantum * PROC_DEFAULT_CONTEXT_TIMER); */
    if(hartid == hart){
        CSR_WRITE("sscratch", p->frame.sscratch);
        ((void(*)(void))spawn_thread_start)();
    }
    //getting the physical address of the starting instruction to run
    u64 physical_addr = virt_to_phys(p->cntl_block.ptable, p->frame.sscratch);

/*     return 1; */
    //start the hart!
    return sbi_start_hart(hartid, spawn_thread_start, physical_addr);

}
