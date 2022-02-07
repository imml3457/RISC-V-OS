#include <syscall.h>
#include <csr.h>
#include <uart.h>
#include <kprint.h>

void unhandled_syscall(u64* s){
    kprint("get gud I haven't included this syscall %U\n", s[17]);
}

void (*syscall_table[])(u64*) = {
    syscall_uart_put,
    syscall_uart_get
};

void syscall_uart_put(u64* s){
    uart_put(s[10]);
}

void syscall_uart_get(u64* s){
    s[10] = uart_get();
}

void syscall_handle(u64 cause, u64 hart){
    u64* mscratch;
    u64  mepc;
    CSR_READ(mscratch, "mscratch");
    CSR_READ(mepc, "mepc");
    syscall_table[mscratch[17]](mscratch);
    CSR_WRITE("mepc", mepc + 4);
}
