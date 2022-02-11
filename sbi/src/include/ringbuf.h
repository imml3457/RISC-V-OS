#ifndef __RINGBUF_H__
#define __RINGBUF_H__

#include <common.h>
#include <lock.h>

#define UART_BUFFER_SIZE 32

struct ring_buffer{
    char ringbuf[UART_BUFFER_SIZE];
    char* head;
    u32 len;
};

void ring_init(struct ring_buffer*);
void ring_push(u8, struct ring_buffer*, Mutex);
u8 ring_pop(struct ring_buffer*, Mutex);

#endif
