#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdint.h>


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

#define ALIGN(x, align)      ((__typeof(x))((((u64)(x)) + (((u64)align) - 1ULL)) & ~(((u64)align) - 1ULL)))
#define IS_ALIGNED(x, align) (!(((u64)(x)) & (((u64)align) - 1ULL)))
#define IS_POWER_OF_TWO(x)   ((x) != 0 && IS_ALIGNED((x), (x)))

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

#endif
