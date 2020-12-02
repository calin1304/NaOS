#include "fs/tar.h"

#include <utils.h>
#include <stddef.h>

void* tar_open(tar_header_t *header, const char *fname)
{
    size_t n = strlen(fname);
    while (header->filename[0] != '\0') {
        if (!strncmp(header->filename, fname, n)) {
            return (char*)header + 512;    
        }
        size_t fsize = oct_to_dec(stoin(header->size, 11));
        header = (char*)header + 512 + fsize + (512 - fsize%512);    
    }
    return NULL;
}