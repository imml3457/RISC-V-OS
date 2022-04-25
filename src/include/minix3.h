#ifndef __MINIX3_H__
#define __MINIX3_H__

#include <common.h>

#define BLOCK_SIZE   1024
#define MINIX3_MAGIC 0x4d5a
struct superblock {
    u32 num_inodes;
    u16 pad0;
    u16 imap_blocks;
    u16 zmap_blocks;
    u16 first_data_zone;
    u16 log_zone_size;
    u16 pad1;
    u32 max_size;
    u32 num_zones;
    u16 magic;
    u16 pad2;
    u16 block_size;
    u8 disk_version;
};

#define NUM_ZONES 10
#define ZONE_SINGLE_INDR 7
#define ZONE_DOUBLE_INDR 8
#define ZONE_TRIPLE_INDR 9


struct inode {
    u16 mode;
    u16 nlinks;
    u16 uid;
    u16 gid;
    u32 size;
    u32 atime;
    u32 mtime;
    u32 ctime;
    u32 zones[NUM_ZONES];
};

#define DIR_ENTRY_NAME_SIZE 60

struct directory_entry{
    u32 inode;
    char name[DIR_ENTRY_NAME_SIZE];
};

#define OFFSET(inode_number, inode_bitmap_block, zone_block) (1024 + BLOCK_SIZE + ((inode_number) - 1) * 64 + (inode_bitmap_block * BLOCK_SIZE) + (zone_block * BLOCK_SIZE))

void init_minix();

#endif
