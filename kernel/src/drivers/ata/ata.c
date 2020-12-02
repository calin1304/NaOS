#include "drivers/ata/ata.h"

#include "io.h"
#include "console.h"

const uint16_t ata_drive_data_port          = 0x1f0;
const uint16_t ata_drive_err_port           = 0x1f1;
const uint16_t ata_sector_count_port        = 0x1f2;
const uint16_t ata_low_lba_port             = 0x1f3;
const uint16_t ata_mid_lba_port             = 0x1f4;
const uint16_t ata_high_lba_port            = 0x1f5;
const uint16_t ata_drive_head_port          = 0x1f6;
const uint16_t ata_cmd_port                 = 0x1f7;
const uint16_t ata_primary_control_register = 0x3F6;


// I/O ports
#define ATA_DATA_REG(x)         (x)
#define ATA_ERROR_REG(x)        ((x) + 1)
#define ATA_FEATURES_REG(x)     ((x) + 1)
#define ATA_SECTOR_COUNT_REG(x) ((x) + 2)
#define ATA_LBA_LOW_REG(x)      ((x) + 3)
#define ATA_LBA_MID_REG(x)      ((x) + 4)
#define ATA_LBA_HIGH_REG(x)     ((x) + 5)
#define ATA_DRIVE_SELECT_REG(x) ((x) + 6)
#define ATA_STATUS_REG(x)       ((x) + 7)
#define ATA_COMMAND_REG(x)      ((x) + 7)

// Control ports
#define ATA_ALTERNATE_STATUS_REG(x) (x)
#define ATA_DEVICE_CONTROL_REG(x)   (x)
#define ATA_DRIVE_ADDRESS_REG(x)    ((x) + 1 )

// Status flags
#define STATUS_ERR 0x01
#define STATUS_DRQ 0x08
#define STATUS_BSY 0x80

// Control register flags
#define CONTROL_nIEN 0x01
#define CONTROL_SRST 0x02
#define CONTROL_HOB  0x40

// Used for 400ns delay mainly after drive select
void ata_delay(uint16_t io_base)
{
    inb(ATA_STATUS_REG(io_base));
    inb(ATA_STATUS_REG(io_base));
    inb(ATA_STATUS_REG(io_base));
    inb(ATA_STATUS_REG(io_base));
}

void ata_select_device(uint16_t io_base, uint16_t slave_select)
{
    uint8_t drive_select = slave_select != 0 ? 0xb0 : 0xa0;
    outb(ATA_DRIVE_SELECT_REG(io_base), drive_select);
    ata_delay(io_base);
}

void ata_reset_(uint16_t control_base)
{
    outb(ATA_DEVICE_CONTROL_REG(control_base), CONTROL_SRST);
    outb(ATA_DEVICE_CONTROL_REG(control_base), 0);
}

ATADeviceType ata_check_type(uint16_t io_base, int slave_select)
{
    ata_select_device(io_base, slave_select);
    ata_delay(io_base);
    // sig[0] = inb(ATA_SECTOR_COUNT_REG(io_base));
    // sig[1] = inb(ATA_LBA_LOW_REG(io_base));
    // sig[2] = inb(ATA_LBA_MID_REG(io_base));
    // sig[3] = inb(ATA_LBA_HIGH_REG(io_base));
    // if (*(uint32_t*)sig == 0x00000101) {
    //     printf("[ATA] Device at %x is a non-packet device\n", io_base);
    // } else if (*(uint32_t*)sig == 0xeb140101) {
    //     printf("[ATA] Device at %x is a packet device\n", io_base);
    // }
    uint8_t sig[2];
    sig[0] = inb(ATA_LBA_MID_REG(io_base));
    sig[1] = inb(ATA_LBA_HIGH_REG(io_base));
    // if (*(uint16_t*)sig == 0xeb14) {
        // printf("Device at %x if PATAPI\n", io_base);
    // }
    switch (*(uint16_t*)sig) {
        case 0xeb14: return ATA_DEVICE_PATAPI;
        case 0x9669: return ATA_DEVICE_SATAPI;
        case 0x0000: return ATA_DEVICE_PATA;
        case 0xc33c: return ATA_DEVICE_SATA;
        default: return ATA_DEVICE_UNKNOWN;
    }
}

ATADeviceType ata_identify(uint16_t io_base, uint16_t control_base, uint8_t slave_select)
{
    ata_select_device(io_base, slave_select);
    if (inb(ATA_STATUS_REG(io_base)) == 0xff) { // Check for floating bus
        // printf("No drive found on bus\n");
        return;
    }
    ata_delay(io_base);
    outb(ATA_SECTOR_COUNT_REG(io_base), 0);
    outb(ATA_LBA_LOW_REG(io_base), 0);
    outb(ATA_LBA_MID_REG(io_base), 0);
    outb(ATA_LBA_HIGH_REG(io_base), 0);
    outb(ATA_COMMAND_REG(io_base), 0xec); // Send IDENTIFY command
    uint8_t status = inb(ATA_STATUS_REG(io_base));
    if (status == 0) {
        // printf("No drive found on port %x, slave_select = %d\n", 
        //         io_base, slave_select);
        return ATA_DEVICE_NONE;
    }
    if (status & STATUS_ERR) {
        // IDENTIFY not supported by ATAPI device, use IDENTIFY_PACKET
        // printf("[#] Drive ident port %x, slave_select %x is ATAPI\n", 
        //     io_base, slave_select);
        return ATA_DEVICE_PATAPI;
    }
    // Wait while device is BUSY
    int wait_for_busy = 5;
    while (inb(ATA_STATUS_REG(io_base) & STATUS_BSY)) { 
    }
    if (inb(ATA_LBA_MID_REG(io_base) || ATA_LBA_HIGH_REG(io_base))) {
        // printf("No ATA device on port %x, slave_select = %d\n", 
        //         io_base, slave_select);
        return ATA_DEVICE_NONE;
    }
    status = inb(ATA_STATUS_REG(io_base)); 
    while (!(status & (STATUS_DRQ | STATUS_ERR))) {
        status = inb(ATA_STATUS_REG(io_base)); 
    }
    if (status & STATUS_ERR) { 
        // printf("[ ! ] Drive error, port %x, slave_select %d\n", 
        //         io_base, slave_select);
        return ATA_DEVICE_UNKNOWN;
    }
    // uint16_t ident[256];
    // for (int i = 0; i < 256; ++i) {
    //     ident[i] = inw(ATA_DATA_REG(io_base));
    // }
    // printf("[#] Drive ident port %x, slave_select %x\n", 
    //         io_base, slave_select);
    // if (ident[83] & 0x400) {
    //     printf("    - 28 bit addressable sectors = %x\n", 
    //             *(uint32_t*)(ident + 60));
    //     printf("    - LBA48 support (48 bit addressable sectors = %x)\n", 
    //             *(uint64_t*)(ident + 100));
    // }
    // printf("Drive status after identify on port %x: %x\n", io_base, status);
}

void ata_init_device(ATADevice * dev, uint16_t io, uint16_t ctrl, uint8_t slave)
{
    ATADeviceType type = ata_identify(ATA_PRIMARY_BUS_IO_BASE, ATA_PRIMARY_BUS_CONTROL_BASE, 1);
    dev->valid = 1;
    dev->type  = type;
    dev->io    = io;
    dev->ctrl  = ctrl;
    dev->slave = slave;
    if (type == ATA_DEVICE_PATAPI) {
        dev->read_sector = atapi_read_sector;
    }
}

void atapi_read_sector(ATADevice *dev, uint32_t lba, uint8_t *buffer)
{

}

void ata_reset(ATADrive *drive)
{
    outb(drive->ports.primaryControlRegister, 4);
    outb(drive->ports.primaryControlRegister, 0);
}

void ata_wait_until_data_available(ATADrive *drive)
{
    while ((inb(drive->ports.commandPort) & 8) == 0);
}

/*
    Source: http://wiki.osdev.org/ATA_read/write_sectors
*/
void ata_readLBA(ATADrive *drive, uint32_t lba, uint8_t sectors, void *dst)
{
    if ((inb(drive->ports.commandPort) & 0x88) == 0) {
        ata_reset(drive);
    }

    outb(drive->ports.headPort, 0xe0 | (((lba >> 24)) & 0x0f)); // set bit 6 for LBA mode
    outb(drive->ports.errorPort, 0);
    outb(drive->ports.sectorCountPort, sectors); // sector count
    outb(drive->ports.lowLBAPort, lba & 0xff);
    outb(drive->ports.midLBAPort, (lba >> 8) & 0xff);
    outb(drive->ports.highLBAPort, (lba >> 16) & 0xff);
    outb(drive->ports.commandPort, 0x20);

    ata_wait_until_data_available(drive);
    for (int i = 0; i < sectors; ++i) {
        for (int j = 0; j < 256; ++j) {
            ata_wait_until_data_available(drive); 
            ((uint16_t*)dst)[i * 256 + j] = inw(0x1f0);
        }
    }
}