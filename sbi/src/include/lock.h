#ifndef __LOCK_H__
#define __LOCK_H__

#include <common.h>

typedef u64 Mutex;
typedef u64 Semaphore;


u64 mutex_trylock(Mutex *mutex);
void mutex_unlock(Mutex *mutex);
u64 semaphore_trydown(Semaphore *sem);
void semaphore_tryup(Semaphore *sem);
void mutex_spinlock(Mutex *mutex);


#endif
