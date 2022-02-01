.section .text.init
.global _start
_start:
        csrr        a0, mhartid
        la          sp, _stack_end
        li          t0, 4096
        mul         t0, t0, a0
        sub         sp, sp, t0
        call        main

.global park
park:
        wfi
        j           park
