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