#include <minix3.h>
#include <imalloc.h>
#include <block.h>
#include <kprint.h>
#include <utils.h>
#include <fs_common.h>
#include <vfs.h>

#define MAX_ZONES 7
u32 if_root = 0;

File* root_file;

static File* minix_populate(u32 node_number, const struct superblock* super, char* buffer, File* parent){
    void* block_buffer = imalloc(BLOCK_SIZE);
    File* tmp_file = imalloc(sizeof(File));
    tmp_file->children = array_make(File*);
    tmp_file->name = imalloc(60 * sizeof(char));

    if(if_root == 0){
        tmp_file->name = "/";
        tmp_file->parent_link = NULL;
        if_root = 1;
    }
    else{
        strcpy(tmp_file->name, buffer);
/*         kprint("what is the name %s\n", tmp_file->name); */
        tmp_file->parent_link = parent;
        array_push(parent->children, tmp_file);
    }
    memset(block_buffer, 0, BLOCK_SIZE);
    struct inode n;
    struct directory_entry dirent;

    u64 byte = OFFSET(node_number, super->imap_blocks, super->zmap_blocks);
    u64 get_block_size = DOWN_ALIGN(byte, BLOCK_SIZE);

    dsk_read(block_buffer, get_block_size, BLOCK_SIZE);

    get_block_size = byte - get_block_size;

    memcpy(&n, block_buffer + get_block_size, sizeof(n));

    tmp_file->inode_number = node_number;
    tmp_file->mode = n.mode;

    if(strlen(buffer) > 0){
/*         kprint("minix test: %s\n", buffer); */
    }
    u32 len;
    if(S_FMT(n.mode) == S_IFDIR){

        for(int i = 0; i < MAX_ZONES; i++){
            if(n.zones[i] == 0){
                //the zone is not used
                continue;
            }

            byte = n.zones[i] * BLOCK_SIZE;

            dsk_read(block_buffer, byte, BLOCK_SIZE);
            for(u32 i = 128; i < n.size; i += sizeof(struct directory_entry)){
                 memcpy(&dirent, block_buffer + i, sizeof(struct directory_entry));
/*                  strcpy(buffer + len + 1, dirent.name); */
                 minix_populate(dirent.inode, super, dirent.name, tmp_file);
            }
        }

        if(n.zones[ZONE_SINGLE_INDR] != 0){
            byte = n.zones[ZONE_SINGLE_INDR] * BLOCK_SIZE;
            dsk_read(block_buffer, byte, BLOCK_SIZE);
        }
    }
    else if(S_FMT(n.mode) == S_IFREG){
    }

/*     imfree(block_buffer); */
/*     kprint("tmp file size %d and name %s\n", array_len(tmp_file->children), tmp_file->name); */
    return tmp_file;
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

    root_file = minix_populate(1, &super, path, NULL);
}
