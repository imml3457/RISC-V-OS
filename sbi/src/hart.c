#include <hart.h>
#include <clint.h>
#include <csr.h>
#include <asm_common.h>
#include <kprint.h>

struct hart_data sbi_hart_data[8];
Mutex hart_locks[8];

u64 h_get_status(u64 hart){
    if(hart >= 8){
        return H_INVALID;
    }
    return sbi_hart_data[hart].status;
}

u64 h_start(u64 hart, u64 target, u64 scratch){
    u64 ret = 1;

    if(!mutex_trylock(&hart_locks[hart])){
        return 0;
    }

    if(sbi_hart_data[hart].status != H_STOPPED){
        mutex_unlock(&hart_locks[hart]);
        return 0;
    }

    else{
        sbi_hart_data[hart].status = H_STARTING;
        sbi_hart_data[hart].target_addr = target;
        sbi_hart_data[hart].scratch = scratch;
        clint_set_msip(hart);
    }
    mutex_unlock(&hart_locks[hart]);
    return ret;
}

u64 h_stop(u64 hart){
    if(!mutex_trylock(&hart_locks[hart])){
        return 0;
    }

    if(sbi_hart_data[hart].status != H_STARTED){
        mutex_unlock(&hart_locks[hart]);
        return 0;
    }
    else{
        sbi_hart_data[hart].status = H_STOPPED;
        CSR_WRITE("mepc", park);
        CSR_WRITE("mstatus", MSTATUS_SET_MACHINE | MSTATUS_SET_MPIE);
        CSR_WRITE("mie", SET_MIE_MSIE);
        mutex_unlock(&hart_locks[hart]);
        MRET();
    }
    mutex_unlock(&hart_locks[hart]);
    return 0;
}

void h_msip(u64 cause, u64 hart){
    (void)cause;
    mutex_spinlock(&hart_locks[hart]);

    clint_clear_msip(hart);
    if(sbi_hart_data[hart].status == H_STARTING){
        CSR_WRITE("mepc", sbi_hart_data[hart].target_addr);
        CSR_WRITE("mstatus", MSTATUS_SET_SUPERVISOR | MSTATUS_SET_MPIE | MSTATUS_FS_INITIAL);
        CSR_WRITE("mie", SET_MIE_MEIE | SET_MIE_SSIE | SET_MIE_STIE | SET_MIE_MTIE);
        CSR_WRITE("mideleg", SET_SIP_SEIP | SET_SIP_SSIP | SET_SIP_STIP);
        CSR_WRITE("medeleg", MEDELEG_ALL);
        CSR_WRITE("sscratch", sbi_hart_data[hart].scratch);
        sbi_hart_data[hart].status = H_STARTED;
    }
    mutex_unlock(&hart_locks[hart]);
    MRET();
}
