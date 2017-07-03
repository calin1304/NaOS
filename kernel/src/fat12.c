#include "fat12.h"
#include "string.h"
#include "ata.h"
#include "clock.h"

struct FAT12RootEntry *root = (struct FAT12RootEntry *)0x1000;
uint8_t *fat = (uint8_t*)0x2500;

struct FAT12RootEntry* fat12_find_file_root_entry(const char *filename)
{
    for (int i = 0; i < 16; ++i) {
        if (strncmp((const char*)root[i].filename, filename, 8) == 0) {
            return root+i;
        }
    }
    return (void*)0x0;
}

uint16_t fat12_get_next_cluster(uint16_t currentCluster)
{
    uint16_t value;
    uint8_t *loc = ((uint8_t*)fat) + (3 * currentCluster)/2;
    if (currentCluster % 2 == 0) {
        value = (*((uint16_t*)loc)) & 0x0fff;
    } else {
        value = (*((uint16_t*)loc)) >> 4;
    }
    return value;
}

void fat12_load_file(struct FAT12RootEntry *re, uint16_t *dst)
{
    uint16_t cluster = re->firstLogicalCluster;
    while (cluster < 0x0fff) {
        ata_read_lba(33+cluster, 1, dst);
        cluster = fat12_get_next_cluster(cluster);
        dst += 256;
    }
}