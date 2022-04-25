#ifndef __VFS_H__
#define __VFS_H__

#include <common.h>
#include <array.h>

enum file_mode{
    DIR,
    FILE
};

typedef struct file_element{
    char* name;
    u32 inode_number;
    array_t children;
    u16 mode;
}File;






#endif
