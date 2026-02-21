#include "cos_syscall.h"
#include "blocks_structures.h"
#include "ext4_functions.h"
#include <stddef.h>
#include <stdint.h>

extern uint64_t lba_start;

static void read_block_content(uint64_t block_addr, uint64_t block_len, size_t *len, uint8_t **data, size_t *offset)
{
    uint8_t block_content[4096] = {0};
    uint16_t i = (*offset >> 12);

    block_addr += i;
    *offset %= 4096;
    for (i = 0; (i < block_len) && (4096 < *len); ++i) {
        read_byte((uint16_t *)block_content, (block_addr << 3) + lba_start, 8);
        cos_memcpy(*data, (uint8_t *)&block_content[*offset], 4096 - *offset);
        *data += (4096 - *offset);
        *len -= (4096 - *offset);
        ++block_addr;
        if (*offset)
            *offset = 0;
    }
    if ((*len <= 4096) && (i < block_len)) {
        read_byte((uint16_t *)block_content, (block_addr << 3) + lba_start, 8);
         if (4096 - *offset < *len) {
            cos_memcpy(*data, (uint8_t *)&block_content[*offset], 4096 - *offset);
            *len -= (4096 - *offset);
        } else {
            cos_memcpy(*data, (uint8_t *)&block_content[*offset], *len);
            *len = 0;
        }
        *offset = 0;
    }
    if (*len && (i < block_len)) {
        ++block_addr;
        read_byte((uint16_t *)block_content, (block_addr << 3) + lba_start, 8);
        cos_memcpy(*data, (uint8_t *)block_content, *len);
        *len = 0;
    }
}

static void write_block_content(uint64_t block_addr, uint64_t block_len, size_t *len, uint8_t **data, size_t *offset)
{
    uint8_t block_content[4096] = {0};
    uint64_t i = *offset >> 12;

    block_addr += i;
    *offset %= 4096;
    read_byte((uint16_t *)block_content, (block_addr << 3) + lba_start, 8);
    for (; (i < block_len) && (4096 < *len); ++i) {
        cos_memcpy((uint8_t *)(&block_content[*offset]), *data, 4096 - *offset);
        write_byte((uint16_t *)block_content, (block_addr << 3) + lba_start, 8);
        *data += (4096 - *offset);
        *len -= (4096 - *offset);
        ++block_addr;
        if (*offset)
            *offset = 0;
    }
    if ((*len <= 4096) && (i < block_len)) {
            read_byte((uint16_t *)block_content, (block_addr << 3) + lba_start, 8);
            if (4096 - *offset < *len) {
                cos_memcpy((uint8_t *)(&block_content[*offset]), *data, 4096 - *offset);
                *len -= (4096 - *offset);
            } else {
                cos_memcpy((uint8_t *)(&block_content[*offset]), *data, *len);
                *len = 0;
            }
            write_byte((uint16_t *)block_content, (block_addr << 3) + lba_start, 8);
            *offset = 0;
    }
    if (*len && (i < block_len)) {
        ++block_addr;
        write_byte((uint16_t *)block_content, (block_addr << 3) + lba_start, 8);
        cos_memcpy(*data, (uint8_t *)block_content, *len);
        *len = 0;
    }
}

/*
//TODO: Reuse this for seeking functionality.
static void write_block_content(uint64_t sectorIndex, uint64_t block_len, size_t *len, uint8_t *content)
{
    uint64_t i = 0;
    uint64_t lba = sectorIndex >> 9;
    uint8_t *new_content =  (uint8_t *)content;
    uint8_t sector_content[SECTOR_SIZE * 2] = {};
    uint64_t x = 0;
    uint64_t y = 0;

    sectorIndex %= 512;
    for (i = 0; ((i + sectorIndex) < SECTOR_SIZE * 2) && (i < *len); ++i)                    //Loop into the content until it reach the limit of SECTOR_SIZE starting with the index or if it reach the content len.
        sector_content[sectorIndex + i] = new_content[i];                                   //Fill the buffer with content at the correct position
    write_byte((uint16_t *)sector_content, lba, 1);                                         //Write byte with buffer content at the correct index
    ++lba;
    *len -= i;                                                                               //Substract to len the lenght of content already written

    for (x = 0; x <= (*len >> 9); ++x) {                                             //Loop for each sector to fill
        memset(sector_content, 0, SECTOR_SIZE * 2);                                     //Empty the buffer
        for (y = 0; (y < SECTOR_SIZE * 2) && ((y + x * SECTOR_SIZE * 2) < *len); ++y)    //Loop into the content until it reach the limit of SECTOR_SIZE or if it reach the content len.
            sector_content[y] = new_content[i + y + x * SECTOR_SIZE * 2];                   //Fill the buffer with content at the correct position
        write_byte((uint16_t *)sector_content, lba + x, 1);                          //Write byte with buffer content
    }
    *len -= x;
}
*/

void read_internal_nodes(extent_internal_t node, size_t *len, uint8_t **content, size_t *offset, uint8_t mode)
{
    uint8_t block_content[4096] = {0};
    extent_head_t head;
    ext4_node_t extracted_node;
    uint64_t block_addr = ((uint64_t)node.ei_leaf_hi << 32) + node.ei_leaf_lo;
    uint64_t size = 4096;
    uint64_t special_case = 0;
    uint64_t j = sizeof(extent_head_t);

    if (*len <= 0)
        return;
    read_block_content(block_addr, 1, &size, (uint8_t **)&block_content, &special_case);
    cos_memcpy(&head, &block_content[0], sizeof(extent_head_t));
    if (head.eh_magic != EXTENT_MAGIC)
        return;
    if (head.eh_entries > head.eh_max)
        return;
    for (uint16_t i = 0; i < head.eh_entries && j < 4096; j += sizeof(extent_internal_t)) {
        cos_memcpy(&extracted_node, &block_content[j], sizeof(extent_internal_t));
        if (!extracted_node.node.ext_internal.ei_unused) {
            read_internal_nodes(extracted_node.node.ext_internal, len, content, offset, mode);
        } else {
            if (extracted_node.node.ext_leaf.ee_len * DEFAULT_BLOCK_SIZE <= *offset) {
                *offset -= extracted_node.node.ext_leaf.ee_len * DEFAULT_BLOCK_SIZE;
                continue;
            }
            block_addr = ((uint64_t)extracted_node.node.ext_leaf.ee_start_hi << 32) + extracted_node.node.ext_leaf.ee_start_lo;
            mode ? read_block_content(block_addr, extracted_node.node.ext_leaf.ee_len, len, content, offset) : write_block_content(block_addr, extracted_node.node.ext_leaf.ee_len, len, content, offset);
        }
    }
}

void read_inode_tree(tree_t *tree, uint8_t **content, size_t *len, size_t *offset, uint8_t mode)
{
    uint64_t block_addr = 0;

    if (*len <= 0)
        return;
    if (tree->ext_head.eh_magic != EXTENT_MAGIC)
        return;
    if (tree->ext_head.eh_entries > tree->ext_head.eh_max)
        return;
        //TO DO: Change this error message to WARN and ignore eh_entries > eh_max.
        //cos_printf("%s\n", "FATAL: Extent header of current inode contains less possible entries than initialised ones (eh_entries > eh_max).");
    //if (tree->ext_head.eh_entries < tree->ext_head.eh_max)
        //cos_printf("%s\n", "WARN: Extent header of current inode contains uninitialised entries (eh_entries < eh_max).");
    
    if (!tree->ext_head.eh_depth) {
        switch (tree->ext_head.eh_entries) {
            case 4:
                if (tree->node4.ext_node_fourth.ei_unused) {
                    read_internal_nodes(tree->node4.ext_node_fourth, len, content, offset, mode);
                    return;
                } else {
                    block_addr = ((uint64_t)tree->node4.ext_leaf_fourth.ee_start_hi << 32) + tree->node4.ext_leaf_fourth.ee_start_lo;
                    mode ? read_block_content(block_addr, tree->node4.ext_leaf_fourth.ee_len, len, content, offset) : write_block_content(block_addr, tree->node4.ext_leaf_fourth.ee_len, len, content, offset);
                    if (*len <= 0)
                        return;
                }
                /* fall through */
            case 3:
                if (tree->node3.ext_node_third.ei_unused) {
                    read_internal_nodes(tree->node3.ext_node_third, len, content, offset, mode);
                    return;
                } else {
                    block_addr = ((uint64_t)tree->node3.ext_leaf_third.ee_start_hi << 32) + tree->node3.ext_leaf_third.ee_start_lo;
                    mode ? read_block_content(block_addr, tree->node3.ext_leaf_third.ee_len, len, content, offset) : write_block_content(block_addr, tree->node3.ext_leaf_third.ee_len, len, content, offset);
                    if (*len <= 0)
                        return;
                }
                /* fall through */
            case 2:
                if (tree->node2.ext_node_second.ei_unused) {
                    read_internal_nodes(tree->node2.ext_node_second, len, content, offset, mode);
                    return;
                } else {
                    block_addr = ((uint64_t)tree->node2.ext_leaf_second.ee_start_hi << 32) + tree->node2.ext_leaf_second.ee_start_lo;
                    mode ? read_block_content(block_addr, tree->node2.ext_leaf_second.ee_len, len, content, offset) : write_block_content(block_addr, tree->node2.ext_leaf_second.ee_len, len, content, offset);
                    if (*len <= 0)
                        return;
                }
                /* fall through */
            case 1:
                if (tree->node1.ext_node_first.ei_unused) {
                    read_internal_nodes(tree->node1.ext_node_first, len, content, offset, mode);
                    return;
                } else {
                    block_addr = ((uint64_t)tree->node1.ext_leaf_first.ee_start_hi << 32) + tree->node1.ext_leaf_first.ee_start_lo;
                    mode ? read_block_content(block_addr, tree->node1.ext_leaf_first.ee_len, len, content, offset) : write_block_content(block_addr, tree->node1.ext_leaf_first.ee_len, len, content, offset);
                }
                break;
            case 0:
                //cos_printf("%s\n", "INFO: Inode contains no data.");
                return;
            default:
                //cos_printf("%s\n", "FATAL: Extent header of current inode contains an invalid number of initialised entries (eh_entries > 4 or eh_entries < 0).");
                return;
        }
    }
    return;
}
