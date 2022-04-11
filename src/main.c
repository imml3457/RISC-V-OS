#include <kprint.h>
#include <common.h>
#include <sbi.h>
#include <utils.h>
#include <tinyshell.h>
#include <page.h>
#include <imalloc.h>
#include <mmu.h>
#include <symbols.h>
#include <pci.h>
#include <plic.h>
#include <trap.h>
#include <rng_driver.h>
#include <block_driver.h>
#include <block.h>
#include <gpu_driver.h>
#include <input_driver.h>
#include <ringbuf.h>
#include <process.h>
#include <elf.h>


struct trapframe SUP_GPREGS[8];

u64 process_stacks[8];

extern u64 idleproc;
extern u64 idleprocsize;

void sup_trap_vector(void);

int main(void){
    kprint_set_putc(sbi_putchar);
    kprint_set_getc(sbi_getchar);
    plic_init();
    CSR_WRITE("stvec", sup_trap_vector);
    CSR_WRITE("sscratch", &(SUP_GPREGS[0].gpregs[0]));
    CSR_WRITE("sie", (1 << 9));
    init_cont_page();
    page_table* p_table = page_cont_falloc(1);
    kernel_page_table = p_table;
    mmu_init();
    imalloc_init();
    initpci();
    pci_set_capes();
    start_gpu();

    for(int i = 0; i < 8; i++){
        process_stacks[i] = (u64)page_cont_falloc(1);
    }

    struct process* p = spawn_new_process_user();
    u8* bytes = imalloc(sizeof(Elf64_Ehdr));
    dsk_read(bytes, 0, sizeof(Elf64_Ehdr));
    Elf64_Ehdr* tmp_header = (Elf64_Ehdr*)bytes;

    u64 program_header_offset = tmp_header->e_phoff;

    u8* ph_bytes = imalloc(sizeof(Elf64_Phdr) * tmp_header->e_phnum);
    dsk_read(ph_bytes, program_header_offset, sizeof(Elf64_Phdr) * tmp_header->e_phnum);

    int status = load_elf_file_from_block(p, bytes, ph_bytes);
    kprint("result of status %d\n", status);
    imfree(ph_bytes);
    imfree(bytes);

    struct process* idle = spawn_new_process_os();

    idle->cntl_block.image_pages = idle->cntl_block.stack_pages;
    idle->cntl_block.number_of_image_pages = 1;
    idle->cntl_block.number_of_stack_pages = 0;
    idle->pid = 0xfffe - 0;
    idle->cntl_block.stack_pages = NULL;

    memcpy(idle->cntl_block.image_pages, (void*)idleproc, idleprocsize);

    mmu_map_single(idle->cntl_block.ptable, DEFAULT_PROCESS_STACK_POINTER, (u64)idle->cntl_block.image_pages, exec);

    idle->frame.sepc = DEFAULT_PROCESS_STACK_POINTER;

    idle->quantum = 50;
    idle->frame.satp = (SV39 | SET_PPN(idle->cntl_block.ptable) | SET_ASID(idle->pid));
    sfence_asid(idle->pid);

    int status = spawn_process_on_hart(idle, 1);
    status = spawn_process_on_hart(p, 5);
/*     WFI(); */

    tsh();


    return 0;
}
