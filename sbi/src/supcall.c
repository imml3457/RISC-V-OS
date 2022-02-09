#include <supcall.h>
#include <csr.h>
#include <uart.h>
#include <kprint.h>

void unhandled_supcall(u64* s){
    kprint("get gud I haven't included this supcall %U\n", s[17]);
}

void (*supcall_table[])(u64*) = {
    supcall_uart_put,
    supcall_uart_get
};

void supcall_uart_put(u64* s){
    uart_put(s[10]);
}

void supcall_uart_get(u64* s){
    //this uses ring_buffer_pop
    s[10] = uart_get_char();
}

void supcall_handle(u64 cause, u64 hart){
    //i do this to suppress warnings
    //the warnings are unused variables
    //the blame is on my jump table for IRQ's
    (void) cause;
    (void) hart;
    u64* mscratch;
    u64  mepc;
    CSR_READ(mscratch, "mscratch");
    CSR_READ(mepc, "mepc");
    supcall_table[mscratch[17]](mscratch);
    CSR_WRITE("mepc", mepc + 4);
}
