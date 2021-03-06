#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdint.h>

#define MRET()  asm volatile("mret")
#define SRET()  asm volatile("sret")
#define WFI()   asm volatile("wfi")

#define MSTATUS_MPP_BIT           11
#define MSTATUS_SET_MACHINE       (3UL << MSTATUS_MPP_BIT)
#define MSTATUS_SET_SUPERVISOR    (1UL << MSTATUS_MPP_BIT)
#define MSTATUS_SET_USER          (0UL << MSTATUS_MPP_BIT)

#define MSTATUS_MPIE_BIT          7
#define MSTATUS_SET_MPIE          (1UL << MSTATUS_MPIE_BIT)

#define MSTATUS_FS_BIT            13
#define MSTATUS_FS_OFF            (0UL << MSTATUS_FS_BIT)
#define MSTATUS_FS_INITIAL        (1UL << MSTATUS_FS_BIT)
#define MSTATUS_FS_CLEAN          (2UL << MSTATUS_FS_BIT)
#define MSTATUS_FS_DIRTY          (3UL << MSTATUS_FS_BIT)

#define SSTATUS_SPP_BIT           8
#define SSTATUS_SET_SUPERVISOR    (1UL << SSTATUS_SPP_BIT)
#define SSTATUS_SET_USER          (0UL << SSTATUS_SPP_BIT)

#define SSTATUS_SPIE_BIT          5
#define SSTATUS_SET_SPIE          (1UL << SSTATUS_SPIE_BIT)

#define SSTATUS_FS_BIT            13
#define SSTATUS_FS_OFF            (0UL << SSTATUS_FS_BIT)
#define SSTATUS_FS_INITIAL        (1UL << SSTATUS_FS_BIT)
#define SSTATUS_FS_CLEAN          (2UL << SSTATUS_FS_BIT)
#define SSTATUS_FS_DIRTY          (3UL << SSTATUS_FS_BIT)

/*         MIP/SIP register               */

//initial bits
#define MEIP_BIT 11
#define SEIP_BIT 9
#define MTIP_BIT 7
#define STIP_BIT 5
#define MSIP_BIT 3
#define SSIP_BIT 1

//setting
#define SET_MIP_MEIP (1UL << MEIP_BIT)
#define SET_MIP_SEIP (1UL << SEIP_BIT)
#define SET_MIP_MTIP (1UL << MTIP_BIT)
#define SET_MIP_STIP (1UL << STIP_BIT)
#define SET_MIP_MSIP (1UL << MSIP_BIT)
#define SET_MIP_SSIP (1UL << SSIP_BIT)


/*        MIE/SIE register              */

#define MEIE_BIT 11
#define SEIE_BIT 9
#define MTIE_BIT 7
#define STIE_BIT 5
#define MSIE_BIT 3
#define SSIE_BIT 1

#define SET_MIE_MEIE (1UL << MEIE_BIT)
#define SET_MIE_SEIE (1UL << SEIE_BIT)
#define SET_MIE_MTIE (1UL << MTIE_BIT)
#define SET_MIE_STIE (1UL << STIE_BIT)
#define SET_MIE_MSIE (1UL << MSIE_BIT)
#define SET_MIE_SSIE (1UL << SSIE_BIT)


#define SET_SIP_SEIP (1UL << SEIP_BIT)
#define SET_SIP_STIP (1UL << STIP_BIT)
#define SET_SIP_SSIP (1UL << SSIP_BIT)

#define SET_SIE_SEIE (1UL << SEIE_BIT)
#define SET_SIE_STIE (1UL << STIE_BIT)
#define SET_SIE_SSIE (1UL << SSIE_BIT)

#define MEX_INSTR_ADDR_MISALIGN   (0)
#define MEX_INSTR_ACCESS_FAULT    (1)
#define MEX_ILLEGAL_INSTR         (2)
#define MEX_BREAKPOINT            (3)
#define MEX_LOAD_ADDR_MISALIGN    (4)
#define MEX_LOAD_ACCESS_FAULT     (5)
#define MEX_STORE_ACCESS_MISALIGN (6)
#define MEX_STORE_ACCESS_FAULT    (7)
#define MEX_ECALL_UMODE           (8)
#define MEX_ECALL_SMODE           (9)
#define MEX_ECALL_MMODE           (11)
#define MEX_INSTR_PAGE_FAULT      (12)
#define MEX_LOAD_PAGE_FAULT       (13)
#define MEX_STORE_PAGE_FAULT      (15)

#define MEDELEG_ALL               (0xB1F7UL)

#define XREG_ZERO                 (0)
#define XREG_RA                   (1)
#define XREG_SP                   (2)
#define XREG_GP                   (3)
#define XREG_TP                   (4)
#define XREG_T0                   (5)
#define XREG_T1                   (6)
#define XREG_T2                   (7)
#define XREG_S0                   (8)
#define XREG_S1                   (9)
#define XREG_A0                   (10)
#define XREG_A1                   (11)
#define XREG_A2                   (12)
#define XREG_A3                   (13)
#define XREG_A4                   (14)
#define XREG_A5                   (15)
#define XREG_A6                   (16)
#define XREG_A7                   (17)
#define XREG_S2                   (18)
#define XREG_S3                   (19)
#define XREG_S4                   (20)
#define XREG_S5                   (21)
#define XREG_S6                   (22)
#define XREG_S7                   (23)
#define XREG_S8                   (24)
#define XREG_S9                   (25)
#define XREG_S10                  (26)
#define XREG_S11                  (27)
#define XREG_T3                   (28)
#define XREG_T4                   (29)
#define XREG_T5                   (30)
#define XREG_T6                   (31)

#define MCAUSE_IS_ASYNC(x)        (((x) >> 63) & 1)
#define MCAUSE_NUM(x)             ((x) & 0xffUL)
#define SCAUSE_IS_ASYNC(x)        (((x) >> 63) & 1)
#define SCAUSE_NUM(x)             ((x) & 0xffUL)


#define ATTR_NAKED_NORET          __attribute__((naked,noreturn))
#define ATTR_NAKED                __attribute__((naked))
#define ATTR_NORET                __attribute__((noreturn))

#define OS_LOAD_ADDR              (0x80050000UL)

#define NULL ((void*)0)

#define UINT(w) uint##w##_t
#define SINT(w) int##w##_t

#define u8  UINT(8 )
#define u16 UINT(16)
#define u32 UINT(32)
#define u64 UINT(64)

#define s8  SINT(8 )
#define s16 SINT(16)
#define s32 SINT(32)
#define s64 SINT(64)


#define UP_ALIGN(x, align) (((x) + (align) - 1) & -(align))
#define DOWN_ALIGN(x, align) ((x) & -(align))


#define KB(x) ((x) * 1024ULL)
#define MB(x) ((x) * 1024ULL * KB(1ULL))
#define GB(x) ((x) * 1024ULL * MB(1ULL))
#define TB(x) ((x) * 1024ULL * GB(1ULL))

#define MAX(a, b) ((a) >= (b) ? (a) : (b))
#define MIN(a, b) ((a) <= (b) ? (a) : (b))
#define LIMIT(x, lower, upper) do { \
    if ((x) < (lower)) {            \
        (x) = (lower);              \
    } else if ((x) > (upper)) {     \
        (x) = (upper);              \
    }                               \
} while (0)

typedef struct unix_time{
    u64 year;
    u64 month;
    u64 day;
    u64 hour;
    u64 min;
    u64 sec;
}unix_time;

#define NS_YEAR  31536000000000000ULL
#define NS_LEAP  31622400000000000ULL
#define NS_DAY   86400000000000ULL

#define CLINT_BASE_ADDRESS 0x2000000
#define CLINT_MTIMECMP_OFFSET 0x4000

#define CLINT_MTIMECMP_INFINITE  0x7FFFFFFFFFFFFFFFUL

#define ECAM_BASE 0x30000000
#define BRIDGE_BASE 0x40000000


#define VIRTIO_VENDOR 0x1af4
#define RNG_DEVICE 0x1044
#define BLOCK_DEVICE 0x1042
#define GPU_DEVICE 0x1050
#define INPUT_DEVICE 0x1052


#endif
