#include <plic.h>
#include <uart.h>
#include <kprint.h>

void plic_set_priority(int interrupt_id, u8 priority)
{
    u32 *base = (u32 *)PLIC_PRIORITY(interrupt_id);
    *base = priority & 0x7;
}
void plic_set_threshold(int hart, u8 priority)
{
    u32 *base = (u32 *)PLIC_THRESHOLD(hart, PLIC_MODE_MACHINE);
    *base = priority & 0x7;
}
void plic_enable(int hart, int interrupt_id)
{
    u32 *base = (u32 *)PLIC_ENABLE(hart, PLIC_MODE_MACHINE);
    base[interrupt_id / 32] |= 1UL << (interrupt_id % 32);
}
void plic_disable(int hart, int interrupt_id)
{
    u32 *base = (u32 *)PLIC_ENABLE(hart, PLIC_MODE_MACHINE);
    base[interrupt_id / 32] &= ~(1UL << (interrupt_id % 32));
}
u32 plic_claim(u64 hart)
{
    u32 *base = (u32 *)PLIC_CLAIM(hart, PLIC_MODE_MACHINE);
    return *base;
}
void plic_complete(int hart, int id)
{
    u32 *base = (u32 *)PLIC_CLAIM(hart, PLIC_MODE_MACHINE);
    *base = id;
}

void plic_init(){
    plic_enable(0, 10);
    plic_set_priority(10, 7);
    plic_set_threshold(0, 0);
}

void plic_handle_irq(u64 cause, u64 hart){
    (void) cause;
    u32 irq = plic_claim(hart);

    kprint("I made it to the plic with hart %d and irq %u\n", hart, irq);
    switch (irq){
        case 10:
            uart_handle_irq();
            plic_complete(hart, irq);
            break;
        default:
            break;

    }

}
