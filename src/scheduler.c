#include <scheduler.h>
#include <lock.h>
#include <imalloc.h>
#include <kprint.h>
#include <csr.h>

Mutex scheduler_mutex = 0;
struct scheduler_elem* process_list_head = NULL;
u16 pids = 1;
int list_iter = 1;
int number_of_list_elems = 0;

void print_list(){
    struct scheduler_elem* iter = process_list_head;

    while(iter != NULL){
        kprint("what is the pid of the current process %d\n", iter->p->pid);
        iter = iter->next;
    }
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

static void scheduler_stop_proc(int hartid){
    struct process* p = what_process_on_hart(hartid);

    if(p == NULL){
        return;
    }

    CSR_READ(p->frame.sepc, "sepc");
    p->on_hart = -1;
}


void schedule(int hartid){
    scheduler_stop_proc(hartid);
    struct process* temp = schedule_next(hartid);
    if(temp == NULL){
        temp = idle_procs[hartid];
    }

    int status = spawn_process_on_hart(temp, hartid);
    kprint("status of the spawn %d\n", status);
}


