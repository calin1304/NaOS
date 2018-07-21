#ifndef FAT12_H
#define FAT12_H

#include <stdint.h>

#include "drivers/ata/ata.h"
#include "vfs.h"

struct FAT12BootSector {
    char ignore1[11];
    uint16_t bytesPerSector;
    uint8_t sectorsPerCluster;
    uint16_t reservedSectors;
    uint8_t numberOfFATs;
    uint16_t rootEntries;
    uint16_t sectorCount;
    uint8_t ignore2;
    uint16_t sectorsPerFAT;
    uint16_t sectorsPerTrack;
    uint16_t headsPerCylinder;
    uint32_t ignore3;
    // uint32_t totalSectorCount; // 0 for FAT12 and FAT16
    // uint16_t ignore4;
    // uint8_t bootSignature;
    // uint32_t volumeId;
    // char volumeLabel[11];
    // uint64_t filesystemType;
} __attribute__((packed));

struct FAT12RootEntry{
    char        filename[8];
    char        extension[3];
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

typedef struct FAT12FileSystem_ {
    FileSystem fs;
    struct FAT12BootSector bootSector;
    ATADrive drive;
} FAT12FileSystem;

void fat12_init(unsigned int volIdx, ATADrive *drive);
FILE* fat12_fopen(const char*, const char*);
size_t fat12_fread(void *ptr, size_t size, size_t count, FILE *f);

#endif