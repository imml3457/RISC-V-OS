#include <elf.h>
#include <kprint.h>
#include <page.h>
#include <utils.h>
#include <mmu.h>
#include <block.h>
#include <imalloc.h>

extern u64 userelfsize;

int load_elf_file_from_block(struct process* proc, u8* buffer_header, u8* buffer_program){

    Elf64_Ehdr* tmp_header = (Elf64_Ehdr*)buffer_header;

    //todo check the magic numbers

    if(tmp_header->e_machine != EM_RISCV || tmp_header->e_type != ET_EXEC){
        return -1;
    }
    int first_page_table_load = 0;

    u64 v_address_start = 0;
    u64 v_address_end = 0;

    Elf64_Phdr* tmp_program_header;
    for(int i = 0; i < tmp_header->e_phnum; i++){
        tmp_program_header = (Elf64_Phdr*)(buffer_program + (i * sizeof(Elf64_Phdr)));

        if(tmp_program_header->p_type == PT_LOAD){
            if(first_page_table_load == 0){
                v_address_end = tmp_program_header->p_vaddr;
                v_address_start = tmp_program_header->p_vaddr;
                first_page_table_load = 1;
            }

            else{
                if(v_address_start > tmp_program_header->p_vaddr){
                    v_address_start = tmp_program_header->p_vaddr;
                }
                else if(v_address_end < (tmp_program_header->p_vaddr + tmp_program_header->p_memsz)){
                    v_address_end = tmp_program_header->p_vaddr + tmp_program_header->p_memsz;
                }
            }
        }
    }
    kprint("the start: %X and the end %X\n", v_address_start, v_address_end);
    if(first_page_table_load == 0){
        return -1;
    }

    proc->frame.sepc = tmp_header->e_entry;

    u64 num_of_end_pages = (((v_address_end + PAGE_SIZE - 1) & -(PAGE_SIZE)));
    u64 num_of_start_pages = (v_address_start & -(PAGE_SIZE));
    u64 total_number_pages = (num_of_end_pages - num_of_start_pages) / PAGE_SIZE;
    proc->cntl_block.number_of_image_pages = (num_of_end_pages - num_of_start_pages) / PAGE_SIZE;
    proc->cntl_block.image_pages = page_cont_falloc(proc->cntl_block.number_of_image_pages);



    u8* tmp_buffer = imalloc(userelfsize);

    dsk_read(tmp_buffer, 0, userelfsize);


    for(int i = 0; i < tmp_header->e_phnum; i++){
        tmp_program_header = (Elf64_Phdr*)(buffer_program + (i * sizeof(Elf64_Phdr)));
        if(tmp_program_header->p_type == PT_LOAD){
            //this code for some reason does not work
            //I will figure it out
/*             tmp_buffer = imalloc(tmp_program_header->p_memsz); */
/*             kprint("what is the offset and file size vs memsize %d %d %d\n", tmp_program_header->p_offset, tmp_program_header->p_filesz, tmp_program_header->p_memsz); */
/*             dsk_read(tmp_buffer, tmp_program_header->p_offset, tmp_program_header->p_memsz); */
/*             imfree(tmp_buffer); */
            memcpy((void*)proc->cntl_block.image_pages + tmp_program_header->p_vaddr - v_address_start,
                    (void*)tmp_buffer + tmp_program_header->p_offset, tmp_program_header->p_memsz);
        }
    }
    mmu_map(proc->cntl_block.ptable, v_address_start, (u64)proc->cntl_block.image_pages, v_address_start + proc->cntl_block.number_of_image_pages * PAGE_SIZE, urwe);
    return 1;
}
