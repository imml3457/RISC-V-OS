#include <ringbuf.h>

void ring_init(struct ring_buffer* buf){
    buf->len = 0;
    buf->head = buf->ringbuf;
}


void ring_push(u8 c, struct ring_buffer* buf, Mutex mutex){
    mutex_spinlock(&mutex);
    if(buf->len < UART_BUFFER_SIZE){
        u32 tmp = (buf->head - buf->ringbuf + buf->len) % UART_BUFFER_SIZE;
        buf->ringbuf[tmp] = c;
        buf->len++;
    }
    //might be used later
/*     else{ */
/*         u32 tmp = (buf->head - buf->ringbuf + buf->len) % UART_BUFFER_SIZE; */
/*         buf->ringbuf[tmp] = c; */
/*         if(buf->head == (buf->ringbuf + UART_BUFFER_SIZE)){ */
/*             buf->head = buf->ringbuf; */
/*         } */
/*         else{ */
/*             buf->head += sizeof(char); */
/*         } */
/*     } */
    mutex_unlock(&mutex);
}

u8 ring_pop(struct ring_buffer* buf, Mutex mutex){
    u8 ret = 0xff;
    mutex_spinlock(&mutex);
    if(buf->len > 0){
        ret = *(buf->head);
        buf->head = buf->head + 1;
        buf->len--;
    }

//this method is for if we wanna wrap around
/*     if(buf->head == (buf->ringbuf + UART_BUFFER_SIZE)){ */
/*         ret = *(buf->head); */
/*         buf->head = buf->ringbuf; */
/*         buf->len--; */
/*     } */
/*     else if(buf->len > 0){ */
/*         ret = 0xff; */
/*     } */
/*     else{ */
/*         ret = *(buf->head); */
/*         buf->head = buf->head + 1; */
/*         buf->len--; */
/*     } */
    mutex_unlock(&mutex);
    return ret;
}
