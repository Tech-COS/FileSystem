#pragma once

#include "ext4_functions.h"

void write_superblock_to_disk(char *disk_address, size_t filesystem_size, size_t *offset, uint64_t timestamp);
void write_block_descriptor(char *disk_address, size_t filesystem_size, size_t *offset);
void write_inode_to_disk(char *disk_address, size_t filesystem_size, size_t *offset, uint64_t timestamp);
