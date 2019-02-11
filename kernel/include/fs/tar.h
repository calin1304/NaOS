#ifndef TAR_H
#define TAR_H

struct tar_header {
    char filename[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12]; // Octal
    char mtime[12]; // Octal
    char checksum[8];
    char typeflag;
    char linkname[100];
    char magic[6];
    char version[2];
    char uname[32];
    char gname[32];
    char devmajor[8];
    char devminor[8];
    char prefix[155];
} __attribute__((packed));

typedef struct tar_header tar_header_t;

void* tar_open(tar_header_t *header, const char *fname);

#endif