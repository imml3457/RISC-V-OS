.section .text.init
.global _start
_start:
        csrr        a0, mhartid
.option push
.option norelax
        la          sp, _stack_end
        la          gp, __global_pointer$
.option pop
        li          t0, 4096
        mul         t0, t0, a0
        sub         sp, sp, t0

        bnez        a0, park

        la          ra, park
        la          t1, main
        csrw        mepc, t1
        li          t1, (3 << 11) | (1 << 7)
        csrw        mstatus, t1
        li          t1, (1 << 11)
        csrw        mie, t1
        la          t1, sbi_trap_vector
        csrw        mtvec, t1
        mret

.global park
park:
        wfi
        j           park
