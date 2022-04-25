.altmacro
.macro loadgp   i, r=t6
        ld    x\i, (\i * 8)(\r)
.endm

.macro savegp   i, r=t6
        sd    x\i, (\i * 8)(\r)
.endm



.section .text
_spawn_kernel_thread:
    csrr     t6, sscratch


    #helpful for remembering certain fields
    # Spawn table fields
    # 0    - gpregs
    # 256  - fpregs
    # 512  - sepc
    # 520  - sstatus
    # 528  - sie
    # 536  - satp
    # 544  - sscratch
    # 552  - stvec
    # 560  - trap_satp
    # 568  - trap_stack


    #getting fields from the sscratch and writing them
    ld     t0, 512(t6)
    csrw   sepc, t0

    ld     t4, 520(t6)
    csrw   sstatus, t4

    ld     t0, 528(t6)
    csrw   sie, t0

    ld     t0, 552(t6)
    csrw   stvec, t0

    ld     t1, 536(t6)

    ld     t6, 544(t6)
    csrw   sscratch, t6

    csrw   satp, t1


    .set i, 1
    .rept 31
        loadgp %i
        .set i, i+1
    .endr

    sret

_spawn_kernel_thread_end:
.align 4

_spawn_trap:
    csrrw  t6, sscratch, t6

    .set i, 1
    .rept 30
        savegp %i
        .set i, i+1
    .endr


    mv     t5, t6
    csrrw  t6, sscratch, t6
    savegp 31, t5


    //this could cause issues
    csrr   t6, sscratch

    ld     t5, 560(t6)
    ld     sp, 568(t6)

    csrw   satp, t5


    call   sup_trap_handler

    csrr   t6, sscratch
    ld     t5, 536(t6)
    csrw   satp, t5


    .set i, 1
    .rept 31
        loadgp %i
        .set i, i+1
    .endr

    sret
_spawn_trap_end:

.section .rodata
.global spawn_thread_start
.global spawn_thread_end
.global spawn_thread_size
spawn_thread_start: .dword _spawn_kernel_thread
spawn_thread_end: .dword _spawn_kernel_thread_end
spawn_thread_size: .dword _spawn_kernel_thread_end - _spawn_kernel_thread

.global spawn_trap_start
.global spawn_trap_end
.global spawn_trap_size
spawn_trap_start: .dword _spawn_trap
spawn_trap_end: .dword _spawn_trap_end
spawn_trap_size: .dword _spawn_trap_end - _spawn_trap
