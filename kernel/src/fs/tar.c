#include "fs/tar.h"

#include <utils.h>
#include <stddef.h>

void* tar_open(tar_header_t *header, const char *fname)
{
    size_t n = strlen(fname);
    // LOG("%p", header);
    while (strncmp(header->filename, fname, n) && header->filename[0] != '\0') {
        // LOG("current filename: %s : %s : %d bytes", header->filename, header->size, oct_to_dec(stoin(header->size, 11)));
        size_t fsize = oct_to_dec(stoin(header->size, 11));
        header = (char*)header + 512 + fsize + (512 - fsize%512);
        // LOG("next header: %p", header);
    }
    return (char*)header + 512;
}