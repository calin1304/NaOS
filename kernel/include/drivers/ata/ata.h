#ifndef ATA_H
#define ATA_H

#include <stdint.h>

typedef enum ATADeviceType_ {
    ATA_DEVICE_PATA,
    ATA_DEVICE_PATAPI,
    ATA_DEVICE_SATA,
    ATA_DEVICE_SATAPI,
    ATA_DEVICE_UNKNOWN,
    ATA_DEVICE_NONE
} ATADeviceType;

#define ATA_PRIMARY_BUS_IO_BASE         0x1f0
#define ATA_PRIMARY_BUS_CONTROL_BASE    0x3f6
#define ATA_SECONDARY_BUS_IO_BASE       0x170
#define ATA_SECONDARY_BUS_CONTROL_BASE  0x376

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