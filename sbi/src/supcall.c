#include <supcall.h>
#include <csr.h>
#include <uart.h>
#include <kprint.h>
#include <common.h>
#include <hart.h>
#include <clint.h>

void unhandled_supcall(u64* s, u64 hart){
    (void)hart;
    kprint("get gud I haven't included this supcall %U\n", s[17]);
}

void (*supcall_table[])(u64*, u64) = {
    supcall_uart_put,
    supcall_uart_get,
    supcall_hart_status,
    supcall_system_off,
    supcall_hart_start,
    supcall_hart_stop,
    supcall_get_time,
    supcall_set_timecmp,
    supcall_add_timecmp,
    supcall_ack_timer,
    supcall_whoami
};

void supcall_uart_put(u64* s, u64 hart){
    (void)hart;
    uart_put(s[XREG_A0]);
}

void supcall_uart_get(u64* s, u64 hart){
    //this uses ring_buffer_pop
    (void)hart;
    s[XREG_A0] = uart_get_char();
}

void supcall_hart_status(u64* s, u64 hart){
    (void)hart;
    s[XREG_A0] = h_get_status(s[XREG_A0]);
}

void supcall_system_off(u64* s, u64 hart){
    (void)s;
    (void)hart;
    *((unsigned short *)0x100000) = 0x5555;
}

void supcall_hart_start(u64* s, u64 hart){
    (void)hart;
    s[XREG_A0] = h_start(s[XREG_A0], s[XREG_A1], s[XREG_A2]);
}

void supcall_hart_stop(u64* s, u64 hart){
    s[XREG_A0] = h_stop(hart);
}

void supcall_get_time(u64* s, u64 hart){
    (void)hart;
    s[XREG_A0] = clint_get_time();
}

void supcall_set_timecmp(u64* s, u64 hart){
    (void)hart;
    clint_set_mtimecmp(s[XREG_A0], s[XREG_A1]);
}

void supcall_add_timecmp(u64* s, u64 hart){
    (void)hart;
    clint_add_mtimecmp(s[XREG_A0], s[XREG_A1]);
}

void supcall_ack_timer(u64* s, u64 hart){
    (void) hart;
    (void) s;
    u64 mip;
    CSR_READ(mip, "mip");
    CSR_WRITE("mip", mip & ~SET_SIP_STIP);
}

void supcall_whoami(u64* s, u64 hart){
    (void)hart;
    s[XREG_A0] = hart;
}

void supcall_handle(u64 cause, u64 hart){
    //i do this to suppress warnings
    //the warnings are unused variables
    //the blame is on my jump table for IRQ's
    (void) cause;
    u64* mscratch;
    u64  mepc;
    CSR_READ(mscratch, "mscratch");
    CSR_READ(mepc, "mepc");
    supcall_table[mscratch[XREG_A7]](mscratch, hart);
    CSR_WRITE("mepc", mepc + 4);
}
