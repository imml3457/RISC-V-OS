#ifndef __LOCK_H__
#define __LOCK_H__

#include <common.h>

typedef u64 Mutex;

typedef struct Semaphore{
    s64 value;
}Semaphore;

/* typedef struct BarrierList { */
/*     struct Process *proc; */
/*     struct BarrierList *next; */
/* }BarrierList; */
/*  */
/* typedef struct Barrier { */
/*     struct BarrierList *head; */
/*     int value; */
/* }Barrier; */

u64 mutex_trylock(Mutex *mutex);
void mutex_unlock(Mutex *mutex);
u64 semaphore_trydown(Semaphore *sem);
void semaphore_tryup(Semaphore *sem);
void mutex_spinlock(Mutex *mutex);
/* void barrier_init(Barrier*); */
/* void barrier_add_process(Barrier*, Proc*); */
/* void barrier_at(Barrier*); */


#endif
