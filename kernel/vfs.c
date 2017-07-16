#include "kernel/include/vfs.h"

#include "libk/include/ctype.h"

FileSystem* registeredFileSystems[26];

FILE* vfs_fopen(const char *filename, const char *mode)
{
    char volLetter = filename[0];
    int volIdx = tolower(volLetter) - 'a';
    FileSystem *fs = registeredFileSystems[volIdx];
    if (fs) {
        return fs->fopen(filename, mode);
    }
    return 0;
}

size_t vfs_fread(void *ptr, size_t size, size_t count, FILE* f)
{
    return f->fread(ptr, size, count, f);
}

void vfs_registerFileSystem(unsigned int idx, FileSystem* fs)
{
    if (fs) { 
        registeredFileSystems[idx] = fs;
    }
}