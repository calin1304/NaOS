#include "fs/fat12/fat12.h"
#include "drivers/ata/ata.h"
#include "clock.h"

#include "libk/include/string.h"
#include "mm/pmm.h"
#include "mm/vmm.h"
#include "kernel/include/vfs.h"

#include <stdio.h>

#define ROOT_DIRECTORY_START_LBA 19
#define FAT_START_LBA 1
#define DATA_START_LBA 33

FAT12FileSystem fat12FS;

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
    // uint16_t cluster = re->firstLogicalCluster;
    // while (cluster < 0x0fff) {
    //     ata_readLBA(31+cluster, 1, dst);
    //     cluster = fat12_get_next_cluster(fatBase, cluster);
    //     dst += 256;
    // }
}

void fat12_init(unsigned int volIdx, ATADrive *drive)
{
    uint8_t *buffer = (uint8_t*)pmm_alloc_block();
    vmm_map_page(buffer, buffer);
    ata_readLBA(drive, 0, 1, buffer);
    const char fsName[] = "FAT12";
    strcpy(fat12FS.fs.name, fsName);
    fat12FS.fs.fopen = fat12_fopen;
    fat12FS.bootSector = *((struct FAT12BootSector*)buffer);
    fat12FS.drive = *drive;
    vmm_free_vaddr_page(buffer);
    pmm_free_block(buffer);
    vfs_registerFileSystem(volIdx, &fat12FS);
}

void convertFilenameTo83(const char *filename, char *name, char *extension)
{
    int noExtension = 0;
    int n = strlen(filename);
    if (n > 11) {
        name[0] = '\0';
        extension[0] = '\0';
        return;
    }
    int i;
    for (i = 0; filename[i] != '\0' && i < 8; ++i) {
        name[i] = toupper(filename[i]);
    }
    for (; i < 8; ++i) {
        name[i] = ' ';
    }
    for (int i = 0; i < 3; ++i) {
        extension[i] = ' ';
    }
}

#include "libk/include/stdio.h"

struct FAT12RootEntry fat12_findRootEntry(const char name[8], const char ext[3])
{
    char buffer[512];
    unsigned int currentLBA = ROOT_DIRECTORY_START_LBA;
    for (;currentLBA < 33; ++currentLBA) {
        ata_readLBA(&(fat12FS.drive), currentLBA, 1, buffer);
        for (int i = 0; i < fat12FS.bootSector.bytesPerSector / sizeof(struct FAT12RootEntry); ++i) {
            struct FAT12RootEntry *e = ((struct FAT12RootEntry*)buffer)+i;
            if (strncmp(e->filename, name, 8) == 0 && strncmp(e->extension, ext, 3) == 0) {
                return *e;
            }
        }
    }
    for (int i = 0; i < 8; ++i) {
        printf("%c", name[i]);
    }
    printf(".");
    for (int i = 0; i < 3; ++i) {
        printf("%c", name[i]);
    }
    printf("|\n");
    printf("Root entry not found\n");
}

void extractFileNameFromPath(const char *path, char *filename)
{
    int n = strlen(path)-1;
    while(n >= 0 && path[n] != '/') {
        --n;
    }
    strcpy(filename, path+n+1);
}

FILE* fat12_fopen(const char* path, const char* mode)
{
    char name[8];
    char ext[3];
    char filename[256];
    extractFileNameFromPath(path, filename);
    convertFilenameTo83(filename, name, ext);
    struct FAT12RootEntry entry = fat12_findRootEntry(name, ext);
    if (!(&entry)) {
        return 0;
    }
    FILE *ret = (FILE*)pmm_alloc_block();
    vmm_map_page(ret, ret);
    ret->size = entry.filesize;
    strcpy(ret->name, filename);
    ret->eof = 0;
    ret->currentPos = 0;
    ret->fread = fat12_fread;
    return ret;
}

unsigned int fat12_getNextCluster(unsigned int currentCluster)
{
    unsigned int loc = (3 * currentCluster)/2;
    unsigned int sector = loc / fat12FS.bootSector.bytesPerSector;
    unsigned int lba = FAT_START_LBA + sector;
    uint8_t buffer[512];
    ata_readLBA(&(fat12FS.drive), lba, 1, buffer);
    unsigned int pos = loc % fat12FS.bootSector.bytesPerSector;
    if (currentCluster % 2 == 0) {
        return *((uint16_t*)(buffer+pos)) & 0x0fff;
    }
    return *((uint16_t*)(buffer+pos)) >> 4;
}

size_t fat12_readData(unsigned int cluster, void *ptr, size_t count)
{
    char buffer[512];
    unsigned int lba = DATA_START_LBA + cluster - 2;
    ata_readLBA(&(fat12FS.drive), lba, 1, buffer);
    memcpy(ptr, buffer, count);
    return count;
}

unsigned int min(unsigned int a, unsigned int b)
{
    return a < b ? a : b;
}

size_t fat12_fread(void *ptr, size_t size, size_t count, FILE *f)
{
    if (f->eof) {
        return;
    }
    if (f->currentPos + count >= f->size) {
        count = f->size - f->currentPos;
        f->eof = 1;
    }
    unsigned int sector = f->currentPos / fat12FS.bootSector.bytesPerSector;
    unsigned int cluster = sector / fat12FS.bootSector.sectorsPerCluster;
    char name[8];
    char ext[3];
    convertFilenameTo83(f->name, name, ext);
    struct FAT12RootEntry entry = fat12_findRootEntry(name, ext);
    unsigned int currentCluster = entry.firstLogicalCluster;
    for (int i = 1; i < cluster; ++i) {
        currentCluster = fat12_getNextCluster(currentCluster);
    }
    unsigned int clusterCount = count / 512 + ((count % 512) != 0);
    int n = 0;
    for (int i = 0; i < clusterCount; ++i) {
        n += fat12_readData(currentCluster, ((uint8_t*)ptr) + 512 * i, min(count, 512));   
        currentCluster = fat12_getNextCluster(currentCluster);
    }
    return n;
}