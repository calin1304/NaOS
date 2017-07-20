#ifndef VFS_H
#define VFS_H

#include <stdint.h>
#include <stddef.h>

#define FILENAME_MAX_LENGTH 256

typedef struct FILE_ {
    unsigned int id;
    unsigned int currentPos;
    unsigned int size;
    unsigned int eof;
    char name[FILENAME_MAX_LENGTH];

    size_t (*fread)(void*, size_t, size_t, struct FILE_*);
} FILE;

typedef struct FileSystem_ {
    char name[10];
    FILE* (*fopen)(const char*, const char*);
} FileSystem;

FILE* vfs_fopen(const char *filename, const char *mode);
size_t vfs_fread(void*, size_t, size_t, FILE*);
void vfs_registerFileSystem(unsigned int, FileSystem*);

#endif