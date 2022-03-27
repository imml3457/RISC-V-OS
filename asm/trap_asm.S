.section .text
.altmacro

.macro savegp i, m=x31
    sd x\i, (8 * \i)(m)
.endm

.macro loadgp i, m=x31
    ld x\i, (8 * \i)(m)
.endm

.align 4

.global sup_trap_vector

sup_trap_vector:
            csrrw         t6, sscratch, t6
            .set i, 1
            .rept 30
                savegp %i
                .set i, i+1
            .endr
            mv            t5, t6
            csrrw         t6, sscratch, t6
            savegp        31, t5
            csrr          t0, sstatus
            srli t0, t0, 13
            andi t0, t0, 3

.option push
.option norelax
            la            sp, _stack_end
            la            gp, __global_pointer$
.option pop
            call          sup_trap_handler
            csrr          t6, sscratch
            .set i, 1
            .rept 31
                loadgp %i
                .set i, i+1
            .endr
            sret
