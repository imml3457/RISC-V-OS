#include <hart.h>

struct hart_data sbi_hart_data[8];


u64 h_get_status(u64 hart){
    if(hart >= 8){
        return H_INVALID;
    }
    return sbi_hart_data[hart].status;
}
