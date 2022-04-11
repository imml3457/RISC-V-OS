.section .text.init
.global _start
_start:
.option push
.option norelax
        la          sp, _stack_end
        la          gp, __global_pointer$
.option pop
        la     t0, main
        csrw   sepc, t0
        la     t0, sup_trap_vector
        csrw   stvec, t0
        li     t0, 1 << 9
        csrw   sie, t0
        li     t0, (1 << 13) | (1 << 8) | (1 << 5)
        csrw   sstatus, t0
//        la     ra, park
//        csrr   a0, sscratch
        sret


.section .text
.global park
.align 4
park:
        wfi
        j           park
park_end:


.section .rodata
.global idleproc
.global idleprocsize

idleproc: .dword park
idleprocsize: .dword park_end - park
/*
.global test
test:
.option push
.option norelax
        la          sp, _stack_end
        la          gp, __global_pointer$
.option pop
        csrr        a0, sscratch
        slli        t0, a0, 13
       sub         sp, sp, t0
        tail        test_hart */
