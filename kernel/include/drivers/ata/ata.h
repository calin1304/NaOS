#ifndef ATA_H
#define ATA_H

#include <stdint.h>

typedef struct ATADrivePorts_ {
    uint16_t dataPort;
    uint16_t errorPort;
    uint16_t sectorCountPort;
    uint16_t lowLBAPort;
    uint16_t midLBAPort;
    uint16_t highLBAPort;
    uint16_t headPort;
    uint16_t commandPort;
    uint16_t primaryControlRegister;
} ATADrivePorts;

typedef struct ATADrive_ {
    ATADrivePorts ports;
} ATADrive;

void ata_readLBA(ATADrive *drive, uint32_t lba, uint8_t sectors, void *dst);

#endif