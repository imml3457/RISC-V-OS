#ifndef __HART_H__
#define __HART_H__
#include <common.h>
#include <lock.h>


enum hart_status{
    H_INVALID,
    H_STOPPED,
    H_STOPPING,
    H_STARTED,
    H_STARTING
};

enum hartpriv{
    H_USER,
    H_SUPERVISOR,
    H_MACHINE
};

struct hart_data{
    enum hart_status status;
    u64 scratch;
    u64 target_addr;
    enum hartpriv mode;
};

extern struct hart_data sbi_hart_data[8];
extern Mutex hart_locks[8];

u64 h_get_status(u64);
u64 h_start(u64, u64, u64);
u64 h_stop(u64);
void h_msip(u64, u64);

#endif
