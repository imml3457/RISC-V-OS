#include <ringbuf.h>

void ring_init(struct ring_buffer* buf){
    buf->len = 0;
    buf->head = buf->ringbuf;
}


void ring_push(char c, struct ring_buffer* buf){
    if(buf->len < UART_BUFFER_SIZE){
        buf->head[buf->len] = c;
        buf->len++;
    }
    else{
        buf->head = buf->ringbuf;
    }
}
