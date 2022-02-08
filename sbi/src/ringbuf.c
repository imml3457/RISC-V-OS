#include <ringbuf.h>
#include <kprint.h>

void ring_init(struct ring_buffer* buf){
    buf->len = 0;
    buf->head = buf->ringbuf;
}


void ring_push(u8 c, struct ring_buffer* buf, Mutex mutex){
    mutex_spinlock(&mutex);
    if(buf->len < UART_BUFFER_SIZE){
        u32 tmp = (buf->head - buf->ringbuf + buf->len) % UART_BUFFER_SIZE;
        buf->ringbuf[tmp] = c;
        buf->len = buf->len + 1;
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
    //this annoys me I wish I could have found a equation that just handles this
    //instead of a couple of if statements
    //but this is the wrap around
    if(buf->len > 0){
        if(buf->head >= (buf->ringbuf + UART_BUFFER_SIZE)){
            buf->head = buf->ringbuf;
            ret = *(buf->head);
            buf->len--;
        }
        else{
            ret = *(buf->head);
            buf->head = buf->head + 1;
            buf->len--;
        }
    }
    mutex_unlock(&mutex);
    return ret;
}
