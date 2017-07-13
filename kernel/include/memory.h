#ifndef MEMORY_H
#define MEMORY_H

struct MemoryMapInfo {
    uint64_t base;
    uint64_t length;
    uint32_t type;
    uint32_t acpi_null;
} __attribute__((packed));

#endif