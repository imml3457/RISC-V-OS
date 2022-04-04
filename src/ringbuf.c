#include <ringbuf.h>
#include <imalloc.h>



struct ring_buffer* ring_init(struct ring_buffer* buffer, u32 cap, u32 behavior){
    buffer = imalloc(sizeof(struct ring_buffer));
    buffer->buf = imalloc(sizeof(u64) * cap);
    buffer->at = 0;
    buffer->num_elems = 0;
    buffer->cap = cap;
    buffer->behavior = behavior;

    return buffer;

}

int ring_push(struct ring_buffer* buffer, void* elem){

    if(buffer->num_elems >= buffer->cap){
        if(buffer->behavior == RING_BUF_DISCARD){
            return 1;
        }
        else if(buffer->behavior == RING_BUF_ERROR){
            return 0;
        }
        else{
            buffer->num_elems -= 1;
            buffer->at = (buffer->at + 1) & buffer->cap;
        }
    }

    u32 temp = (buffer->at + buffer->num_elems) % buffer->cap;

    buffer->buf[temp] = elem;
    buffer->num_elems += 1;

    return 1;
}

int ring_pop(struct ring_buffer* buffer, void** elem){
    if(buffer->num_elems == 0){
        return 0;
    }

    *elem = buffer->buf[buffer->at];
    buffer->at = (buffer->at + 1) % buffer->cap;
    buffer->num_elems -= 1;

    return 1;
}
