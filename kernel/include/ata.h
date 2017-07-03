#ifndef ATA_H
#define ATA_H

#include <stdint.h>

void ata_read_lba(uint32_t lba, uint8_t sectors, uint16_t *dst);

#endif