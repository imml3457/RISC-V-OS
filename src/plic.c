#include <plic.h>
#include <kprint.h>

void plic_set_priority(int interrupt_id, u8 priority)
{
    u32 *base = (u32 *)PLIC_PRIORITY(interrupt_id);
    *base = priority & 0x7;
}
void plic_set_threshold(int hart, u8 priority)
{
    u32 *base = (u32 *)PLIC_THRESHOLD(hart, PLIC_MODE_SUPERVISOR);
    *base = priority & 0x7;
}
void plic_enable(int hart, int interrupt_id)
{
    u32 *base = (u32 *)PLIC_ENABLE(hart, PLIC_MODE_SUPERVISOR);
    base[interrupt_id / 32] |= 1UL << (interrupt_id % 32);
}
void plic_disable(int hart, int interrupt_id)
{
    u32 *base = (u32 *)PLIC_ENABLE(hart, PLIC_MODE_SUPERVISOR);
    base[interrupt_id / 32] &= ~(1UL << (interrupt_id % 32));
}
u32 plic_claim(u64 hart)
{
    u32 *base = (u32 *)PLIC_CLAIM(hart, PLIC_MODE_SUPERVISOR);
    return *base;
}
void plic_complete(int hart, int id)
{
    u32 *base = (u32 *)PLIC_CLAIM(hart, PLIC_MODE_SUPERVISOR);
    *base = id;
}

void plic_init(){
    plic_enable(0, 32);
    plic_set_priority(32, 7);
    plic_enable(0, 33);
    plic_set_priority(33, 7);
    plic_enable(0, 34);
    plic_set_priority(34, 7);
    plic_enable(0, 35);
    plic_set_priority(35, 7);
    plic_set_threshold(0, 0);
}

void plic_handle_irq(u64 cause){
    (void) cause;
    u32 irq = plic_claim(0);

    switch (irq){
        case 32:
            kprint("in plic\n");
            break;
        case 33:
            kprint("in plic\n");
            break;
        case 34:
            kprint("in plic\n");
            break;
        case 35:
            kprint("in plic\n");
            break;
        default:
            break;

    }
}
