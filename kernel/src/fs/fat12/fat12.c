#include "fs/fat12/fat12.h"
#include "drivers/ata/ata.h"
#include "clock.h"

#include <string.h>
#include "mm/pmm.h"
#include "mm/vmm.h"
#include "vfs.h"
#include "utils.h"

#include <libk/stdio.h>
#include <ctype.h>

#define ROOT_DIRECTORY_START_LBA 19
#define FAT_START_LBA 1
#define DATA_START_LBA 33

FAT12FileSystem fat12FS;

void fat12_init(unsigned int volIdx, ATADrive *drive)
{
    uint8_t *buffer = (uint8_t*)malloc(512);
    ata_readLBA(drive, 0, 1, buffer);
    const char fsName[] = "FAT12";
    strcpy(fat12FS.fs.name, fsName);
    fat12FS.fs.fopen = fat12_fopen;
    fat12FS.bootSector = *((struct FAT12BootSector*)buffer);
    fat12FS.drive = *drive;
    // free(buffer);
}

FAT12FileSystem* getFAT12Driver()
{
    return &fat12FS;
}

static void convertFilenameTo83(const char *filename, char *name, char *extension)
{   
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

static struct FAT12RootEntry fat12_findRootEntry(const char name[8], const char ext[3])
{
    struct FAT12RootEntry ret;
    memset(&ret, 0, sizeof(struct FAT12RootEntry));
    char buffer[512];
    unsigned int currentLBA = ROOT_DIRECTORY_START_LBA;
    for (;currentLBA < 33; ++currentLBA) {
        ata_readLBA(&(fat12FS.drive), currentLBA, 1, buffer);
        for (unsigned int i = 0; i < fat12FS.bootSector.bytesPerSector / sizeof(struct FAT12RootEntry); ++i) {
            ret = *(((struct FAT12RootEntry*)buffer)+i);
            if (strncmp(ret.filename, name, 8) == 0 && strncmp(ret.extension, ext, 3) == 0) {
                return ret;
            }
        }
    }
    return ret;
}

static void extractFileNameFromPath(const char *path, char *filename)
{
    int n = strlen(path)-1;
    while(n >= 0 && path[n] != '/') {
        --n;
    }
    strcpy(filename, path+n+1);
}

FILE* fat12_fopen(const char* path, const char* mode __attribute__ ((unused)))
{
    char name[8];
    char ext[3];
    char filename[256];
    extractFileNameFromPath(path, filename);
    convertFilenameTo83(filename, name, ext);
    struct FAT12RootEntry entry = fat12_findRootEntry(name, ext);
    if (!entry.attributes) {
        return 0;
    }
    FILE *ret = malloc(sizeof(FILE));
    ret->size = entry.filesize;
    strcpy(ret->name, filename);
    ret->eof = 0;
    ret->currentPos = 0;
    ret->fread = fat12_fread;
    return ret;
}

static unsigned int fat12_getNextCluster(unsigned int currentCluster)
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

static size_t fat12_readData(unsigned int cluster, void *ptr, size_t count)
{
    char buffer[512];
    unsigned int lba = DATA_START_LBA + cluster - 2;
    ata_readLBA(&(fat12FS.drive), lba, 1, buffer);
    memcpy(ptr, buffer, count);
    return count;
}


size_t fat12_fread(void *ptr, size_t size __attribute__ ((unused)), size_t count, FILE *f)
{
    if (f->eof) {
        return 0;
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
    for (unsigned int i = 1; i < cluster; ++i) {
        currentCluster = fat12_getNextCluster(currentCluster);
    }
    unsigned int clusterCount = count / 512 + ((count % 512) != 0);
    int n = 0;
    for (unsigned int i = 0; i < clusterCount; ++i) {
        n += fat12_readData(currentCluster, ((uint8_t*)ptr) + 512 * i, min(count, 512));   
        currentCluster = fat12_getNextCluster(currentCluster);
    }
    return n;
}