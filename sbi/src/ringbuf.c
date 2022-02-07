#include <ringbuf.h>

void ring_init(struct ring_buffer* buf){
    buf->len = 0;
    buf->head = buf->ringbuf;
}


void ring_push(char c, struct ring_buffer* buf){
    if(buf->len < UART_BUFFER_SIZE){
        u32 tmp = (buf->head - buf->ringbuf + buf->len) % UART_BUFFER_SIZE;
        buf->ringbuf[tmp] = c;
        buf->len++;
    }
    else{
        u32 tmp = (buf->head - buf->ringbuf + buf->len) % UART_BUFFER_SIZE;
        buf->ringbuf[tmp] = c;
        if(buf->head == (buf->ringbuf + UART_BUFFER_SIZE)){
            buf->head = buf->ringbuf;
        }
        else{
            buf->head += sizeof(char);
        }
    }
}

char ring_pop(struct ring_buffer* buf){
    char ret;
    if(buf->head == (buf->ringbuf + UART_BUFFER_SIZE)){
        ret = *(buf->head);
        buf->head = buf->ringbuf;
        buf->len--;
    }
    else{
        ret = *(buf->head);
        buf->head = buf->head + 1;
        buf->len--;
    }
    return ret;
}
