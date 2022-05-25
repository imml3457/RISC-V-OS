#ifndef __VFS_H__
#define __VFS_H__

#include <common.h>
#include <array.h>

enum file_mode{
    DIR,
    FILE
};

struct file_element;

typedef struct file_element{
    char* name;
    u32 inode_number;
    array_t children;
    u16 mode;
    struct file_element* parent_link;
}File;


extern File* root_file;

File* get_file(char*);

#endif
