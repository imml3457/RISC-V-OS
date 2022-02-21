#include <lock.h>
#include <kprint.h>


u64 mutex_trylock(Mutex *mutex){
    u64 old;
    //wait
    asm volatile("amoswap.w.aq %0, %1, (%2)" : "=r"(old) : "r"(1), "r"(mutex));
    //1 is locked
    return old != 1;
}

void mutex_unlock(Mutex *mutex){
    //so we can dump whatever value it was before
    //we don't really care what it was
    //we just wanna unlock
    asm volatile("amoswap.w.rl zero, zero, (%0)" :: "r"(mutex));
}

void mutex_spinlock(Mutex *mutex){

    //spin until unlocked
    while (!mutex_trylock(mutex));
}

u64 semaphore_trydown(Semaphore *sem) {
    int old;
    asm volatile ("amoadd.w %0, %1, (%2)" : "=r"(old) : "r"(-1), "r"(&sem->value));
    if (old <= 0) {
        //we need to get back to 0 if the semaphore was negative
        semaphore_tryup(sem);
    }
    return old > 0;
}
void semaphore_tryup(Semaphore *sem) {
    asm volatile ("amoadd.w zero, %0, (%1)" : : "r"(1), "r"(&sem->value));
}


/* void barrier_init(struct Barrier *barrier) { */
/*     barrier->head = NULL; */
/*     barrier->value = 0; */
/* } */
/* void barrier_add_process(struct Barrier *barrier, struct Process *proc) { */
/*     struct BarrierList *bl; */
/*     bl = kmalloc(sizeof(*bl)); */
/*     bl->proc = proc; */
/*     bl->next = barrier->head; */
/*     barrier->head = bl; */
/*     barrier->value += 1; */
/* } */
/* void barrier_at(struct Barrier *barrier) { */
/*     int old; */
/*     asm volatile("amoadd.w %0, %1, (%2)"  */
/*                    : "=r"(old)  */
/*                    : "r"(-1), "r"(&barrier->value) */
/*                 ); */
/*     if (old <= 1) { */
/*         struct BarrierList *bl, *next; */
/*         for (bl = barrier->head;NULL != bl;bl = next) { */
/*             next = bl->next; */
/*             bl->proc->state = STATE_RUNNING; */
/*             kfree(bl); */
/*         } */
/*         barrier->head = NULL; */
/*     } */
/* } */
