#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include <common.h>
#include <process.h>

struct scheduler_elem{
    struct process* p;
    struct scheduler_elem* next;
};

void sched_cfs_init();

void schedule(int);

void schedule_add(struct process*);

void schedule_add_cfs(struct process*);

void schedule_remove(struct process*);

void print_list();

extern struct scheduler_elem* process_list_head;


#endif
