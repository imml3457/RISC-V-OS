#ifndef __HART_H__
#define __HART_H__
#include <common.h>


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
    enum hartpriv mode;
    u64 target_addr;
};

extern struct hart_data sbi_hart_data[8];

u64 h_get_status(u64);

#endif
