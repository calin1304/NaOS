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

struct ATADevice_;
typedef void (*readFcn)(struct ATADevice_*, uint16_t, uint16_t, uint8_t);

typedef struct ATADevice_ {
    ATADeviceType   type;
    uint16_t        io;
    uint16_t        ctrl;
    uint8_t         slave;
    uint8_t         valid;
    readFcn         read_sector;
} ATADevice;

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

ATADeviceType ata_check_type(uint16_t io_base, int slave_select);
ATADeviceType ata_identify(uint16_t io_base, uint16_t control_base, 
                      uint8_t slave_select);
void ata_init_device(ATADevice * dev, uint16_t io, uint16_t ctrl, uint8_t slave);
void ata_readLBA(ATADrive *drive, uint32_t lba, uint8_t sectors, void *dst);

void atapi_read_sector(ATADevice *dev, uint32_t lba, uint8_t *buffer);
#endif