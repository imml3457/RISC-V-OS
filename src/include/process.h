#ifndef __PROCESS_H__
#define __PROCESS_H__

#include <mmu.h>
#include <common.h>

struct resource_control_block {
    void* image_pages;
    void* stack_pages;
    void* heap_pages;
    void* file_descriptors;
    u64 number_of_image_pages;
    u64 number_of_stack_pages;
    u64 number_of_heap_pages;
    // environment contains things
    // like PATH, CWD, UID, GID
    page_table *ptable;
};
struct proc_stats {
    u64 vruntime;
    u64 switches;
};
enum process_state {
    PS_DEAD = 0,
    PS_RUNNING = 1,
    PS_SLEEPING = 2,
    PS_WAITING = 3,
    PS_NONE = 4
};

struct proc_frame{
    u64 gpregs[32];
    //not doing floating point yet
    u64 fpregs[32];
    u64 sepc;
    u64 sstatus;
    u64 sie;
    u64 satp;
    u64 sscratch;
    u64 stvec;
    u64 trap_satp;
    u64 trap_stack;
};


struct process {
    struct proc_frame frame;
    u16 state;
    struct resource_control_block cntl_block;
    struct proc_stats proc_stats;
    u64  sleep_until;
    u16  quantum;
    u16  pid;
    int  on_hart; // -1 if not running on a HART
    int os_flag;
};


#define PROC_DEFAULT_CONTEXT_TIMER 10000
#define PROC_DEFAULT_QUANTUM 100
#define DEFAULT_PROCESS_VIRTUAL_ADDR 0x400000
#define DEFAULT_PROCESS_STACK_POINTER 0x1beef0000UL


extern struct process *processes[];
extern u64 process_stacks[];

extern struct process *idle_procs[];

struct process* process_new();

struct process* spawn_new_process_user();
struct process* spawn_new_process_os();

int spawn_process_on_hart(struct process* p, int hartid);

#define what_process_on_hart(hartid) (processes[hartid])
#define set_process_on_hart(hartid, p) (processes[hartid] = p)

#endif
