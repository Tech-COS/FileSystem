#include "filesystem_shared_macros.h"
#include "cos_syscall.h"

uint8_t discover_disk(uint64_t *max_sectors, bool is_iso)
{
    uint16_t read_data[256] = {0};
    uint64_t result = 0;
    uint8_t params = 0;

    if (!is_iso)
        detect_drive(read_data);

    for (int i = 100; i < 104; ++i) {
        *max_sectors <<= 16;
        *max_sectors |= read_data[i];
    }

    if (((read_data[83] >> 10) & 1) == 1)
        params |= LBA48_BIT;

    if (((read_data[93] >> 11) & 1) == 1)
        params |= UDMA_SUPPORT_BIT;

    result = read_data[60];
    result <<= 16;
    result |= read_data[61];
    if (result)
        params |= LBA28_BIT;
    return params;
}
