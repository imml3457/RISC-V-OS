#include <vfs.h>
#include <utils.h>
#include <imalloc.h>
#include <kprint.h>

File* get_file(char* path){
    if(strlen(path) < 1){
        return NULL;
    }
    char* path_cpy = imalloc(strlen(path));
    char* tmp_name = imalloc(1024);
    strcpy(path_cpy, path);

    if(path_cpy[0] != '/'){
        return NULL;
    }

    path_cpy++;

    File* tmp_file = root_file;
    int num_bytes_to_copy = 0;
    int num_bytes_read = 1;

    while(*path_cpy != 0){
        if(*path_cpy == '/'){
            kprint("what is numbytes read and copy %d %d\n", num_bytes_read, num_bytes_to_copy);
            memcpy(tmp_name, path + num_bytes_read, num_bytes_to_copy);
            num_bytes_read += num_bytes_to_copy;
            num_bytes_to_copy = 0;
            num_bytes_to_copy--;
            num_bytes_read++;
            kprint("tmp_name %s\n", tmp_name);
            memset(tmp_name, 0, 1024);
        }
        path_cpy++;
        num_bytes_to_copy++;
    }


    kprint("what is numbytes read and copy %d %d\n", num_bytes_read, num_bytes_to_copy);
    memcpy(tmp_name, path + num_bytes_read, num_bytes_to_copy);

    kprint("tmp_name %s\n", tmp_name);

}
