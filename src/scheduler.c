#include <scheduler.h>
#include <lock.h>
#include <imalloc.h>
#include <kprint.h>
#include <csr.h>
#include "tree.h"
#include <sbi.h>

Mutex scheduler_mutex = 0;
struct scheduler_elem* process_list_head = NULL;
u16 pids = 1;
int list_iter = 1;
int number_of_list_elems = 0;

int not_so_fair_initial = 0;



struct cfs_tree_node{
    struct process* p;
    u64 virt_runtime;
};

struct cfs_tree_node current_run[8];

typedef struct process* proc;
use_tree(u64, proc);

tree(u64, proc) cfs_tree;
tree_it(u64, proc) cfs_iter;

void sched_cfs_init(){
    cfs_tree = tree_make(u64, proc);
}

void print_list(){
    kprint("what is the len of the tree %d\n", tree_len(cfs_tree));
}

void schedule_add(struct process* p){
    mutex_spinlock(&scheduler_mutex);
    struct scheduler_elem* temp_elem = imalloc(sizeof(struct scheduler_elem));
    if(p->pid == 0){
        if(process_list_head == NULL){
            p->pid = pids;
        }

        else{
            p->pid = pids;
        }

        p->frame.satp = (SV39 | SET_PPN(p->cntl_block.ptable) | SET_ASID(p->pid));
        sfence_asid(p->pid);
    }

    if(process_list_head == NULL){
        temp_elem->p = p;
        temp_elem->next = NULL;
        process_list_head = temp_elem;
    }
    else{
        struct scheduler_elem* iter = process_list_head;
        while(iter->next != NULL){
            kprint("here\n");
            iter = iter->next;
        }
        temp_elem->p = p;
        temp_elem->next = NULL;
        iter->next = temp_elem;
    }
    pids++;
    number_of_list_elems++;
    mutex_unlock(&scheduler_mutex);
}


static struct process* schedule_next(int hartid){
    struct scheduler_elem* iter = process_list_head;
    mutex_spinlock(&scheduler_mutex);
    for(int i = 1; i < list_iter; i++){
        iter = iter->next;
    }

    while(iter != NULL){
        if(iter->p->state != PS_DEAD && iter->p->on_hart == -1){
            if(iter->p->state == PS_RUNNING){
                iter->p->on_hart = hartid;
                kprint("here in sched next\n");
                mutex_unlock(&scheduler_mutex);
                return iter->p;
            }
        }
        list_iter++;
        iter = iter->next;
    }

    if(list_iter == number_of_list_elems){
        list_iter = 0;
    }


    //got to the bottom of the list with no process that can be ran
    mutex_unlock(&scheduler_mutex);
    return NULL;
}

void schedule_add_cfs(struct process* p){
    mutex_spinlock(&scheduler_mutex);
    if(p->pid == 0){
        p->pid = pids;
        p->frame.satp = (SV39 | SET_PPN(p->cntl_block.ptable) | SET_ASID(p->pid));
        sfence_asid(p->pid);
    }
    pids++;
    tree_insert(cfs_tree, not_so_fair_initial, p);
    not_so_fair_initial++;
    mutex_unlock(&scheduler_mutex);
}

static struct process* schedule_next_cfs(int hartid){
    mutex_spinlock(&scheduler_mutex);
    cfs_iter = tree_begin(cfs_tree);
    while(tree_it_good(cfs_iter)){
        struct process* temp = tree_it_val(cfs_iter);
        if(temp->state != PS_DEAD && temp->on_hart == -1){
            if(temp->state == PS_RUNNING){
                temp->on_hart = hartid;
                current_run[hartid].p = temp;
                current_run[hartid].virt_runtime = sbi_get_time();
                tree_delete(cfs_tree, tree_it_key(cfs_iter));
                mutex_unlock(&scheduler_mutex);
                return temp;
            }
        }
        tree_it_next(cfs_iter);
    }
    return NULL;
}

static void scheduler_stop_proc(int hartid){
    struct process* p = what_process_on_hart(hartid);
    if(p == NULL){
        return;
    }
    mutex_spinlock(&scheduler_mutex);
    tree_insert(cfs_tree, sbi_get_time() - current_run[hartid].virt_runtime, current_run[hartid].p);
    current_run[hartid].p = NULL;
    current_run[hartid].virt_runtime = 0;
    mutex_unlock(&scheduler_mutex);
    CSR_READ(p->frame.sepc, "sepc");
    p->on_hart = -1;
}


void schedule(int hartid){
    scheduler_stop_proc(hartid);
    struct process* temp = schedule_next_cfs(hartid);
    if(temp == NULL){
        temp = idle_procs[hartid];
    }
    int status = spawn_process_on_hart(temp, hartid);
/*     kprint("status of the spawn %d\n", status); */
}



