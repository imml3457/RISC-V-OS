#ifndef __FS_COMMON_H__
#define __FS_COMMON_H__

#include <common.h>

#define S_FMT(x)       (S_IFMT & (x))

#define S_IFMT        0170000 /* These bits determine file type.  */

/* File types.  */
#define S_IFDIR       0040000 /* Directory.  */
#define S_IFCHR       0020000 /* Character device.  */
#define S_IFBLK       0060000 /* Block device.  */
#define S_IFREG       0100000 /* Regular file.  */
#define S_IFIFO       0010000 /* FIFO.  */
#define S_IFLNK       0120000 /* Symbolic link.  */
#define S_IFSOCK      0140000 /* Socket.  */

/* POSIX.1b objects.  Note that these macros always evaluate to zero.  But
   they do it by enforcing the correct use of the macros.  */
#define __S_TYPEISMQ(buf)  ((buf)->st_mode - (buf)->st_mode)
#define __S_TYPEISSEM(buf) ((buf)->st_mode - (buf)->st_mode)
#define __S_TYPEISSHM(buf) ((buf)->st_mode - (buf)->st_mode)

/* Protection bits.  */

#define S_ISUID       04000   /* Set user ID on execution.  */
#define S_ISGID       02000   /* Set group ID on execution.  */
#define S_ISVTX       01000   /* Save swapped text after use (sticky).  */
#define S_IREAD       0400    /* Read by owner.  */
#define S_IWRITE      0200    /* Write by owner.  */
#define S_IEXEC       0100    /* Execute by owner.  */

struct stat
{
    int st_dev;              /* Device. */
    int st_ino;                 /* File serial number. */
    int st_nlink;             /* Link count. */
    short st_mode;             /* File mode.  */
    short st_uid;            /* User ID of the file's owner. */
    short st_gid;            /* Group ID of the file's group. */
    short st_rdev;           /* Device number, if device. */
    u32 st_size;    /* Size of file, in bytes. */
    u32 st_blksize; /* Optimal block size for I/O.  */
    u32 st_blocks;  /* Number 512-byte blocks allocated. */
    u32 st_atime;   /* Time of last access.  */
    u32 st_mtime;   /* Time of last modification. */
    u32 st_ctime;   /* Time of last status change. */
};


#endif
