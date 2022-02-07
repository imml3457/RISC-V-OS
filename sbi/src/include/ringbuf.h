#ifndef __RINGBUF_H__
#define __RINGBUF_H__

#include <common.h>

#define UART_BUFFER_SIZE 32

struct ring_buffer{
    char ringbuf[UART_BUFFER_SIZE];
    char* head;
    u32 len;
};

void ring_init(struct ring_buffer*);
void ring_push(char, struct ring_buffer*);
char ring_pop(struct ring_buffer*);

#endif
