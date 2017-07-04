#include <fs/fat12/fat12.h>
#include "ata.h"
#include "clock.h"

#include "libc/include/string.h"

const unsigned int ROOT_ENTRIES_COUNT = 224;

struct FAT12RootEntry* fat12_find_file_root_entry(struct FAT12RootEntry *rootDirectory, const char *filename)
{
    for (unsigned int i = 0; i < ROOT_ENTRIES_COUNT; ++i) {
        if (strncmp((const char*)rootDirectory[i].filename, filename, 8) == 0) {
            return rootDirectory+i;
        }
    }
    return (void*)0x0;
}

uint16_t fat12_get_next_cluster(uint8_t *fatBase, uint16_t currentCluster)
{
    uint16_t value;
    uint8_t *loc = ((uint8_t*)fatBase) + (3 * currentCluster)/2;
    if (currentCluster % 2 == 0) {
        value = (*((uint16_t*)loc)) & 0x0fff;
    } else {
        value = (*((uint16_t*)loc)) >> 4;
    }
    return value;
}

void fat12_load_file(uint8_t *fatBase, struct FAT12RootEntry *re, uint16_t *dst)
{
    uint16_t cluster = re->firstLogicalCluster;
    while (cluster < 0x0fff) {
        ata_read_lba(31+cluster, 1, dst);
        cluster = fat12_get_next_cluster(fatBase, cluster);
        dst += 256;
    }
}