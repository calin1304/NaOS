#ifndef FAT12_H
#define FAT12_H

#include <stdint.h>

struct FAT12RootEntry{
    uint8_t     filename[8];
    uint8_t     extension[3];
    uint8_t     attributes;
    uint16_t    reserved;
    uint16_t    creationTime;
    uint16_t    creationDate;
    uint16_t    lastAccessDate;
    uint16_t    ignore;
    uint16_t    lastWriteTime;
    uint16_t    lastWriteDate;
    uint16_t    firstLogicalCluster;
    uint32_t    filesize;
} __attribute__((packed));

struct FAT12RootEntry* fat12_find_file_root_entry(struct FAT12RootEntry *rootDirectory, const char *filename);
uint16_t fat12_get_next_cluster(uint8_t *fatBase, uint16_t currentCluster);
void fat12_load_file(uint8_t *fatBase, struct FAT12RootEntry *re, uint16_t *dst);

#endif