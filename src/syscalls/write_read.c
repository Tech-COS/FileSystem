#include "../include/kernel/asm.h"
#include "../include/kernel/interrupts.h"
#include <stdint.h>

void wait_for_ready(uint8_t flags_set, uint8_t flags_not_set)
{
    uint8_t value = 0xFF & ~flags_set;

    //Sleep 400ns until real sleep is made.
    value = cos_inb(0x1F7);
    value = cos_inb(0x1F7);
    value = cos_inb(0x1F7);
    value = cos_inb(0x1F7);
    while (!(value & flags_set) || (value & flags_not_set))
    {
        value = cos_inb(0x1F7);
    }
}

static void write_loop(uint16_t data[], uint16_t sectorcount)
{
    uint64_t nb_bytes = 256 * sectorcount;

    for (uint16_t i = 0; i < nb_bytes; ++i) {
        if (i && !(i & 0xFF))
            wait_for_ready(0x8, 0x80);
        cos_out16(0x1F0, data[i]);
    }
}

void write_byte(uint16_t data[], uint64_t lba, uint16_t sectorcount)
{
    cos_outb(0x1F6, 0x40);

    cos_outb(0x1F2, (sectorcount >> 8) & 0xFF);
    cos_outb(0x1F3, (lba >> 24) & 0xFF);
    cos_outb(0x1F4, (lba >> 32) & 0xFF);
    cos_outb(0x1F5, (lba >> 40) & 0xFF);

    cos_outb(0x1F2, sectorcount & 0xFF);
    cos_outb(0x1F3, lba & 0xFF);
    cos_outb(0x1F4, (lba >> 8) & 0xFF);
    cos_outb(0x1F5, (lba >> 16) & 0xFF);

    cos_outb(0x1F7,0x34);

    wait_for_ready(0x40, 0x80);
    write_loop(data, sectorcount);

    cos_outb(0x1F7, 0xE7);
    wait_for_ready(0x40, 0x80);
}

void read_loop(uint16_t data[], uint16_t sectorcount)
{
    uint64_t nb_bytes = 256 * sectorcount;

    for (uint64_t i = 0; i < nb_bytes; ++i) {
        if (i && !(i & 0xFF))
            wait_for_ready(0x8, 0x80);
        data[i] = cos_in16(0x1F0);
    }
}

void read_byte(uint16_t data[], uint64_t lba, uint16_t sectorcount)
{
    cos_outb(0x1F6, 0x40);

    cos_outb(0x1F2, (sectorcount >> 8) & 0xFF);
    cos_outb(0x1F3, (lba >> 24) & 0xFF);
    cos_outb(0x1F4, (lba >> 32) & 0xFF);
    cos_outb(0x1F5, (lba >> 40) & 0xFF);

    cos_outb(0x1F2, sectorcount & 0xFF);
    cos_outb(0x1F3, lba & 0xFF);
    cos_outb(0x1F4, (lba >> 8) & 0xFF);
    cos_outb(0x1F5, (lba >> 16) & 0xFF);

    cos_outb(0x1F7,0x24);

    wait_for_ready(0x40, 0x80);
    read_loop(data, sectorcount);
}
