#include "ext4_functions.h"

void cos_memcpy(void *dest_buffer, void *source_buffer, size_t data_size)
{
    uint8_t *src =  (uint8_t *)source_buffer;
    uint8_t *dest = (uint8_t *)dest_buffer;
    size_t size = 0;

    while (data_size > size) {
        dest[size] = src[size];
        ++size;
    }
}
