.section .text.init
.global _start
_start:
        csrr        a0, mhartid
        la          sp, _stack_end
        li          t0, 4096
        mul         t0, t0, a0
        sub         sp, sp, t0

        la          t1, main
        csrw        mepc, t1
        li          t1, (3 << 11) | (1 << 7)
        csrw        mstatus, t1
        mret



.global park
park:
        wfi
        j           park
