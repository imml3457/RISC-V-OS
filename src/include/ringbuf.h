#ifndef __RING_BUF_H__
#define __RING_BUF_H__

#include <common.h>

#define RING_BUF_OVERWRITE 0
#define RING_BUF_DISCARD 1
#define RING_BUF_ERROR 2

struct ring_buffer{
    u32 at;
    u32 num_elems;
    u32 cap;
    u32 behavior;
    void** buf;
};


struct ring_buffer* ring_init(struct ring_buffer*, u32 cap, u32);
int ring_push(struct ring_buffer*, void*);
int ring_pop(struct ring_buffer*, void**);

#endif
