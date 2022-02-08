#include <lock.h>


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

