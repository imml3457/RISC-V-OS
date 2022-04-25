#include <minix3.h>
#include <imalloc.h>
#include <block.h>
#include <kprint.h>
#include <utils.h>
#include <fs_common.h>
#include <vfs.h>

#define MAX_ZONES 7

static void minix_populate(u32 node_number, const struct superblock* super, char* buffer){
    void* block_buffer = imalloc(BLOCK_SIZE);
    memset(block_buffer, 0, BLOCK_SIZE);
    struct inode n;
    struct directory_entry dirent;

    u64 byte = OFFSET(node_number, super->imap_blocks, super->zmap_blocks);
    u64 get_block_size = DOWN_ALIGN(byte, BLOCK_SIZE);

    dsk_read(block_buffer, get_block_size, BLOCK_SIZE);

    get_block_size = byte - get_block_size;

    memcpy(&n, block_buffer + get_block_size, sizeof(n));

    if(strlen(buffer) > 0){
        kprint("minix test: %s\n", buffer);
    }
    u32 len;
    if(S_FMT(n.mode) == S_IFDIR){
        len = strlen(buffer);
        strcpy(buffer + len, "/");

        for(int i = 0; i < MAX_ZONES; i++){
            if(n.zones[i] == 0){
                //the zone is not used
                continue;
            }

            byte = n.zones[i] * BLOCK_SIZE;

            dsk_read(block_buffer, byte, BLOCK_SIZE);
            for(u32 i = 128; i < n.size; i += sizeof(struct directory_entry)){
                 memcpy(&dirent, block_buffer + i, sizeof(struct directory_entry));
                 strcpy(buffer + len + 1, dirent.name);
                 minix_populate(dirent.inode, super, buffer);
            }
        }

        if(n.zones[ZONE_SINGLE_INDR] != 0){
            kprint("in the indirect\n");
            byte = n.zones[ZONE_SINGLE_INDR] * BLOCK_SIZE;
            dsk_read(block_buffer, byte, BLOCK_SIZE);
        }

    }
    else if(S_FMT(n.mode) == S_IFREG){
        len = strlen(buffer);
    }

    imfree(block_buffer);
}


void init_minix(){

    struct superblock super;

    char* path = imalloc(BLOCK_SIZE);
    void* block_buffer = imalloc(BLOCK_SIZE);

    dsk_read(block_buffer, BLOCK_SIZE, BLOCK_SIZE);

    memcpy(&super, block_buffer, sizeof(super));
    if(super.magic != MINIX3_MAGIC){
        kprint("I am not getting a minix3 filesystem\n");
    }

    minix_populate(1, &super, path);

}
