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

        tail main

.global park
park:
        wfi
        j           park
