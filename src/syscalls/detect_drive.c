#include "cos_syscall.h"
#include "../include/kernel/asm.h"
#include <stdbool.h>
#include <stdint.h>

static bool select_master(uint8_t command_master)
{
    cos_outb(0x1F6, command_master);
    cos_outb(0x1F5, 0);
    cos_outb(0x1F4, 0);
    cos_outb(0x1F3, 0);
    cos_outb(0x1F7, 0xEC);
    if (!cos_inb(0x1F7))
        return false;
    return true;
}

static bool check_non_ata_status_ports(void)
{
    uint8_t status = cos_inb(0x1F4);

    if (status)
        return false;
    status = cos_inb(0x1F5);

    if (status)
        return false;
    return true;
}

static bool wait_for_information(void)
{
    uint8_t value = 0xFF;

    cos_inb(0x1F7);
    cos_inb(0x1F7);
    cos_inb(0x1F7);
    cos_inb(0x1F7);
    while (value & 0x4)
    {
        value = cos_inb(0x1F7);
        if (value & 0x1)
            return false;
    }
    return true;
}

bool detect_drive(uint16_t data[])
{
    if (!select_master(0xA0))
        return false;
    wait_for_ready(0x40, 0x80);
    if (!check_non_ata_status_ports())
        return false;
    if (!wait_for_information())
        return false;
    read_loop(data, 1);
    return true;
}
