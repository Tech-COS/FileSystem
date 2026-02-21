/*
** EPITECH PROJECT, 2025
** COS
** File description:
** inodes_header
*/

#include "../include/blocks_structures.h"
#include "../include/data_types.h"
#include "../include/ext4_functions.h"
#include "../include/cos_syscall.h"
#include <stddef.h>
#include <stdint.h>
#include "../../include/kernel/cos_time.h"
#include "../../include/kernel/lib/string.h"
#include "kernel/term/tty.h"
#include "../include/wrapper.h"
#include "../include/env.h"
#include "../include/stat.h"

void unix_to_datetime(uint64_t timestamp, datetime_t *dt);
void format_datetime(const datetime_t *dt, char *buffer, uint64_t buffer_size);

uint64_t lba_start = 0;

static uint64_t convert64(uint32_t high, uint32_t low)
{
    return (uint64_t)(((uint64_t)high << 32) | low);
}

static void read_superblock(superblock_t *sb)
{
    uint16_t buffer[512];

    read_byte(&buffer[0], 2 + lba_start, 1);
    read_byte(&buffer[256], 3 + lba_start, 1);
    cos_memcpy(sb, buffer, sizeof(superblock_t));
}

static void read_block_descriptor_table(block_descriptor_t *block_table, uint64_t inode_block_nb)
{
    uint16_t buffer[256];
    uint8_t lba = 8 + (uint8_t)(inode_block_nb >> 3); //Divided by 8

    read_byte(buffer, lba + lba_start, 1);
    cos_memcpy(block_table, &buffer[(inode_block_nb & 7) << 5], sizeof(block_descriptor_t)); //Modulus 8 and multiplied by 32
}

static uint64_t read_inode_metadata(uint64_t inode_offset, block_descriptor_t *block_group, inode_t *inode_metadata, bool is_content_read, bool mode)
{
    uint64_t inode_table_lba = ((block_group->bg_inode_table_hi + block_group->bg_inode_table_lo) << 3) + (inode_offset >> 1); //4K blocks IDs are stored in inode_table vars of the structure, as such it must be multiplied by 8.
    uint16_t buffer[256] = {0};

    read_byte(buffer, inode_table_lba + lba_start, 1);
    cos_memcpy(inode_metadata, &buffer[(inode_offset & 1) << 7], sizeof(inode_t)); // Modulus 2 and multiplied by 128
    if (is_content_read) {
        if (mode == READ_MODE)
            inode_metadata->i_atime = handle_unix_timestamp(0);
        else
            inode_metadata->i_mtime = handle_unix_timestamp(0);
        cos_memcpy(&buffer[((inode_offset) & 1) << 7], inode_metadata, sizeof(inode_t));
        write_byte(buffer, inode_table_lba + lba_start, 1);
    }
    return inode_table_lba + lba_start;
}

static void write_inode_metadata(uint64_t inode_offset, block_descriptor_t *block_group, inode_t *inode_metadata, bool mode)
{
    uint64_t inode_table_lba = ((block_group->bg_inode_table_hi + block_group->bg_inode_table_lo) << 3) + (inode_offset >> 1); //4K blocks IDs are stored in inode_table vars of the structure, as such it must be multiplied by 8.
    uint16_t buffer[256] = {0};
    uint64_t unix_timestamp = handle_unix_timestamp(0);

    read_byte(buffer, inode_table_lba + lba_start, 1);
    if (mode == WRITE_MODE)
        inode_metadata->i_mtime = unix_timestamp;
    inode_metadata->i_ctime = unix_timestamp;
    cos_memcpy(&buffer[((inode_offset) & 1) << 7], inode_metadata, sizeof(inode_t));
    write_byte(buffer, inode_table_lba + lba_start, 1);
}

static uint64_t get_inode_block_group_number(superblock_t *sb, uint64_t inode_nb)
{
    return (uint64_t)((inode_nb - 1) / sb->s_inodes_per_group);
}

static uint64_t get_inode_table_offset(superblock_t *sb, uint64_t inode_nb)
{
    return (uint64_t)((inode_nb - 1) % sb->s_inodes_per_group);
}

static void update_super_inode_count(superblock_t *sb, uint64_t inode_count)
{
    uint16_t buffer[512];

    sb->s_free_inodes_count += (uint32_t)inode_count;
    cos_memcpy(buffer, sb, sizeof(superblock_t));
    write_byte(&buffer[0], 2 + lba_start, 1);
    write_byte(&buffer[256], 3 + lba_start, 1);
}

static void update_group_inode_count(block_descriptor_t *block_desc, uint64_t inode_block_nb, uint64_t inode_count)
{
    uint16_t buffer[256];
    uint8_t lba = 8 + (uint8_t)(inode_block_nb >> 3); //Divided by 8
    uint32_t free_inodes_block = (uint32_t)((uint32_t)block_desc->bg_free_inodes_count_hi << 16) + (uint32_t)block_desc->bg_free_inodes_count_lo + (uint32_t)inode_count;

    block_desc->bg_free_inodes_count_lo = (uint16_t)(free_inodes_block & 0xFFFF);
    block_desc->bg_free_inodes_count_hi = (uint16_t)((free_inodes_block >> 16) & 0xFFFF);
    cos_memcpy(&buffer[(inode_block_nb & 7) << 5], (uint8_t *)block_desc, sizeof(block_descriptor_t)); //Modulus 8 and multiplied by 32
    write_byte(buffer, lba + lba_start, 1);
}

static void update_inode_bitmap(block_descriptor_t block_desc, uint64_t inode_nb)
{
    uint8_t filesystem_block[4096] = {0};
    uint64_t bitmap_lba = 0;

    bitmap_lba = convert64(block_desc.bg_inode_bitmap_hi, block_desc.bg_inode_bitmap_lo) * 8;
    read_byte((uint16_t *)filesystem_block, bitmap_lba + lba_start, 8);
    filesystem_block[inode_nb / 8] |= 1 << ((inode_nb % 8) - 1);
    write_byte((uint16_t *)filesystem_block, bitmap_lba + lba_start, 8);
}

static void initialise_null_inode(inode_t metadata, uint64_t inode_offset, uint64_t inode_table_lba)
{
    extent_head_t head;
    uint16_t buffer[256] = {0};
    uint64_t unix_timestamp = handle_unix_timestamp(0);

    head.eh_depth = 0;
    head.eh_entries = 0;
    head.eh_magic = 0xF30A;
    head.eh_max = 4;
    head.eh_generation = 0;

    metadata.i_links_count = 1;
    metadata.i_mode = 0x8000 | 0x100 | 0x80 | 0x20 | 0x4;
    metadata.i_flags = 0x80000;
    metadata.i_blocks_lo = 0;
    metadata.osd2.linux2.l_i_blocks_high = 0;
    metadata.i_size_lo = 0;
    metadata.i_size_high = 0;
    metadata.i_extra_isize = 32;
    metadata.i_generation = head.eh_generation;
    metadata.i_gid = 1000;
    metadata.osd2.linux2.l_i_gid_high = 0;
    metadata.i_uid = 1000;
    metadata.osd2.linux2.l_i_uid_high = 0;
    metadata.i_atime = unix_timestamp;
    metadata.i_ctime = unix_timestamp;
    metadata.i_mtime = unix_timestamp;
    metadata.i_crtime = unix_timestamp;
    metadata.i_ctime_extra = 0;
    metadata.i_mtime_extra = 0;
    metadata.i_atime_extra = 0;
    metadata.i_crtime_extra = 0;
    metadata.i_dtime = 0;
    metadata.i_file_acl_lo = (uint32_t) 0;
    metadata.osd2.linux2.l_i_file_acl_high = (uint16_t) 0;
    metadata.osd2.linux2.l_i_reserved = 0;
    metadata.i_obso_faddr = 0;
    metadata.osd1.linux1.l_i_version = (uint32_t)0;
    metadata.i_version_hi = (uint32_t)0;
    metadata.i_projid = 0;
    memset(metadata.i_cos_reserved, '\0', sizeof(uint8_t) * 96);
    cos_memcpy(&metadata.i_block[0], &head, sizeof(extent_head_t));

    read_byte(buffer, inode_table_lba, 1);
    cos_memcpy(&buffer[(inode_offset & 1) << 7], &metadata, sizeof(inode_t));
    write_byte(buffer, inode_table_lba, 1);
}

uint64_t get_first_null_inode(void)
{
    uint64_t nb_blocks = 0;
    uint64_t j = 0;
    uint64_t k = 0;
    uint64_t inode_offset = 0;
    uint64_t inode_table_lba = 0;
    inode_t metadata;
    block_descriptor_t block_desc;
    superblock_t sb;

    read_superblock(&sb);
    nb_blocks = sb.s_inodes_count / sb.s_inodes_per_group;
    k = sb.s_first_ino;
    for (uint64_t i = 0; 0 < nb_blocks; ++i) {
        read_block_descriptor_table(&block_desc, i);
        for (j = 0; j < sb.s_inodes_per_group; ++j) {
            ++k;
            inode_offset = get_inode_table_offset(&sb, k);
            inode_table_lba = read_inode_metadata(inode_offset, &block_desc, &metadata, CONTENT_ACCESSED, READ_MODE);
            if (!metadata.i_links_count) {
                initialise_null_inode(metadata, inode_offset, inode_table_lba);
                update_inode_bitmap(block_desc, k);
                update_group_inode_count(&block_desc, i,-1);
                update_super_inode_count(&sb, -1);
                return k;
            }
        }
    }
    return 0;
}

uint64_t update_inode_links(uint64_t inode_nb, int64_t links, uint16_t mode, uint16_t moved_file)
{
    uint64_t inode_offset = 0;
    uint64_t inode_block = 0;
    inode_t metadata;
    block_descriptor_t block_desc;
    superblock_t sb;

    read_superblock(&sb);
    inode_block = get_inode_block_group_number(&sb, inode_nb);
    inode_offset = get_inode_table_offset(&sb, inode_nb);
    read_block_descriptor_table(&block_desc, inode_block);
    read_inode_metadata(inode_offset, &block_desc, &metadata, CONTENT_NOT_ACCESSED, READ_MODE);
    if (mode == FILE_TYPE_DIRECTORY) {
        if (moved_file == FILE_TYPE_DIRECTORY)
            metadata.i_links_count += links;
        write_inode_metadata(inode_offset, &block_desc, &metadata, WRITE_MODE);
    } else {
        write_inode_metadata(inode_offset, &block_desc, &metadata, READ_MODE);
    }
    return 0;
}

static void format_permissions(uint16_t mode, char *perm_str)
{
    char file_type = '-';

    if (mode & 0x4000) file_type = 'd';      // Directory
    else if (mode & 0x8000) file_type = '-'; // Regular file
    else if (mode & 0x2000) file_type = 'c'; // Character device
    else if (mode & 0x6000) file_type = 'b'; // Block device
    else if (mode & 0x1000) file_type = 'p'; // FIFO
    else if (mode & 0xA000) file_type = 'l'; // Symbolic link
    else if (mode & 0xC000) file_type = 's'; // Socket

    perm_str[0] = file_type;

    // Owner
    perm_str[1] = (mode & 0x100) ? 'r' : '-';
    perm_str[2] = (mode & 0x080) ? 'w' : '-';
    perm_str[3] = (mode & 0x040) ? 'x' : '-';

    // Group
    perm_str[4] = (mode & 0x020) ? 'r' : '-';
    perm_str[5] = (mode & 0x010) ? 'w' : '-';
    perm_str[6] = (mode & 0x008) ? 'x' : '-';

    // Other
    perm_str[7] = (mode & 0x004) ? 'r' : '-';
    perm_str[8] = (mode & 0x002) ? 'w' : '-';
    perm_str[9] = (mode & 0x001) ? 'x' : '-';

    perm_str[10] = '\0';
}
static void get_device_numbers(superblock_t *sb, uint16_t *major, uint16_t *minor)
{
    uint32_t device_hash = 0;

    for (int i = 0; i < 16; i++) {
        device_hash += sb->s_uuid[i];
    }

    *major = 8 + (device_hash % 4);  // Majeur  8-11
    *minor = 1 + (device_hash % 16); // Mineur  1-16
}

static const char* get_username(uint32_t uid)
{
    switch (uid) {
        case 0: return "root";
        case 1000: return "user";
        case 1001: return "admin";
        default: return "unknown";
    }
}

static const char* get_groupname(uint32_t gid)
{
    switch (gid) {
        case 0: return "root";
        case 1000: return "user";
        case 1001: return "admin";
        default: return "unknown";
    }
}

uint64_t get_inode_nb_blocks_taken(uint64_t inode_nb)
{
    superblock_t sb;
    block_descriptor_t block_group;
    inode_t metadata;
    uint64_t inode_block;
    uint64_t inode_offset;

    read_superblock(&sb);
    inode_block = get_inode_block_group_number(&sb, inode_nb);
    inode_offset = get_inode_table_offset(&sb, inode_nb);
    read_block_descriptor_table(&block_group, inode_block);
    read_inode_metadata(inode_offset, &block_group, &metadata, CONTENT_ACCESSED, READ_MODE);
    return convert64(metadata.osd2.linux2.l_i_blocks_high, metadata.i_blocks_lo);
}

void read_wrapper(uint64_t inode_nb, uint8_t *content, size_t len, size_t offset, bool is_content_read)
{
    superblock_t sb;
    block_descriptor_t block_group;
    inode_t metadata;
    tree_t tree;
    uint64_t inode_block;
    uint64_t inode_offset;

    read_superblock(&sb);
    inode_block = get_inode_block_group_number(&sb, inode_nb);
    inode_offset = get_inode_table_offset(&sb, inode_nb);
    read_block_descriptor_table(&block_group, inode_block);
    read_inode_metadata(inode_offset, &block_group, &metadata, is_content_read, READ_MODE);
    tree = *(struct ext_tree *)metadata.i_block;
    read_inode_tree(&tree, &content, &len, &offset, READ_MODE);
}

bool read(const char *path, uint8_t *content, size_t len, size_t offset)
{
uint8_t buffer[4096] = {0};
    dirent2_t dir;
    uint32_t source_dir_inode;
    char source_dir_path[256] = {0};
    char source_filename[EXT4_NAME_LEN] = {0};
    short i = 0;

    //cos_printf("=== DEBUG READ FUNCTION ===\n");
    //cos_printf("Original path: '%s'\n", path);

    extract_directory_path(path, source_dir_path);
    extract_filename(path, source_filename);

    //cos_printf("Directory path: '%s'\n", source_dir_path);
    //cos_printf("Filename: '%s'\n", source_filename);

    source_dir_inode = find_directory_inode(source_dir_path, true, NULL);
    //cos_printf("Directory inode: %d\n", source_dir_inode);

    if (source_dir_inode == 0) {
        //cos_printf("read: cannot access '%s': No such file or directory\n", source_dir_path);
        return false;
    }

    //cos_printf("Reading directory content...\n");
    read_wrapper(source_dir_inode, buffer, DEFAULT_BLOCK_SIZE, offset, CONTENT_NOT_ACCESSED);

    //cos_printf("Scanning directory entries:\n");
    do {
        cos_memcpy(&dir, &buffer[i], 8);

        if (!dir.name_len) {
            //cos_printf("End of directory entries (name_len = 0)\n");
            break;
        }

        cos_memcpy(dir.name, &buffer[8 + i], dir.name_len);
        dir.name[dir.name_len] = '\0';  // Assurer la fin de chaîne
        //cos_printf("Comparing: '%s' == '%s' ? %d\n", source_filename, dir.name, cos_strcmp(source_filename, dir.name));

        //cos_printf("Entry: name='%s', type=%d, inode=%d, rec_len=%d\n",
        //           dir.name, dir.file_type, dir.inode, dir.rec_len);

        if (cos_strcmp(source_filename, dir.name) == 0) {
            //cos_printf("Found matching file! Reading content...\n");
            read_wrapper(dir.inode, content, len, 0, CONTENT_ACCESSED);
            //cos_printf("=== END DEBUG ===\n");
            return true;
        }

        memset(dir.name, '\0', EXT4_NAME_LEN);
        i += dir.rec_len;

        // Sécurité pour éviter une boucle infinie
        if (i >= 4096) {
            //cos_printf("Reached end of buffer\n");
            break;
        }

    } while (i < 4096);

    //cos_printf("File not found in directory\n");
    //cos_printf("=== END DEBUG ===\n");
    return false;
}


static uint64_t read_inode_bitmap(block_descriptor_t block_desc, uint64_t inode_block, superblock_t *sb)
{
    uint8_t block_bitmap[4096] = {0};
    uint64_t bitmap_block_chosen = 0;
    uint64_t bitmap_lba = 0;

    bitmap_lba = convert64(block_desc.bg_inode_bitmap_hi, block_desc.bg_inode_bitmap_lo) * 8;
    read_byte((uint16_t *)block_bitmap, bitmap_lba + lba_start, 8);

    for (uint16_t i = 0; i < 4096; ++i) {
        if (((block_bitmap[i / 8]) >> (i % 8)) & 1)
            continue;
        bitmap_block_chosen = i * 4096 + inode_block * sb->s_blocks_per_group * 4096;
        break;
    }
    return bitmap_block_chosen;
}

void write_wrapper(uint64_t inode_nb, uint8_t *content, size_t len, size_t offset)
{
    uint8_t *content_ptr = content;
    uint64_t bitmap_block_chosen = 0;
    superblock_t sb;
    block_descriptor_t block_group;
    inode_t metadata;
    tree_t tree;
    uint64_t inode_block;
    uint64_t inode_offset;

    read_superblock(&sb);
    inode_block = get_inode_block_group_number(&sb, inode_nb);
    inode_offset = get_inode_table_offset(&sb, inode_nb);
    read_block_descriptor_table(&block_group, inode_block);
    read_inode_metadata(inode_offset, &block_group, &metadata, CONTENT_ACCESSED, WRITE_MODE);
    tree = *(struct ext_tree *)metadata.i_block;
    if (!tree.ext_head.eh_entries) {
        bitmap_block_chosen = read_inode_bitmap(block_group, inode_nb, &sb);
        tree.node1.ext_leaf_first.ee_start_lo = bitmap_block_chosen & 0xFFFFFFFF;
        tree.node1.ext_leaf_first.ee_start_hi = (bitmap_block_chosen >> 32) & 0xFFFFFFFF;
        tree.node1.ext_leaf_first.ee_block = 0;
        tree.node1.ext_leaf_first.ee_len = 1;
        tree.ext_head.eh_entries = 1;
        cos_memcpy(&metadata.i_block[0], &tree, 60);
        write_inode_metadata(inode_offset, &block_group, &metadata, WRITE_MODE);
    }
    read_inode_tree(&tree, &content_ptr, &len, &offset, WRITE_MODE);
}

// Sysacll stat
int cos_stat(const char *path, stat_t *statbuf)
{
    uint8_t buffer[4096] = {0};
    dirent2_t dir;
    uint32_t source_dir_inode;
    char source_dir_path[256] = {0};
    char source_filename[EXT4_NAME_LEN] = {0};
    short i = 0;
    inode_t file_inode;
    superblock_t sb;
    block_descriptor_t block_group;
    uint64_t inode_block;
    uint64_t inode_offset;
    uint16_t major, minor;

    if (!path || !statbuf) {
        return -1;
    }

    extract_directory_path(path, source_dir_path);
    extract_filename(path, source_filename);
    
    source_dir_inode = find_directory_inode(source_dir_path, true, NULL);
    
    if (source_dir_inode == 0) {
        return -2;
    }

    read_wrapper(source_dir_inode, buffer, DEFAULT_BLOCK_SIZE, 0, true);
    
    do {
        cos_memcpy(&dir, &buffer[i], 8);

        if (!dir.name_len)
            break;

        cos_memcpy(dir.name, &buffer[8 + i], dir.name_len);
        dir.name[dir.name_len] = '\0';
        
        if (cos_strcmp(source_filename, dir.name) == 0) {
            read_superblock(&sb);
            inode_block = get_inode_block_group_number(&sb, dir.inode);
            inode_offset = get_inode_table_offset(&sb, dir.inode);
            read_block_descriptor_table(&block_group, inode_block);
            read_inode_metadata(inode_offset, &block_group, &file_inode, true, false);
            
            get_device_numbers(&sb, &major, &minor);
            
            statbuf->st_dev = (uint64_t)((major << 8) | minor);
            statbuf->st_ino = dir.inode;
            statbuf->st_mode = file_inode.i_mode;
            statbuf->st_nlink = file_inode.i_links_count;
            statbuf->st_uid = convert64(file_inode.osd2.linux2.l_i_uid_high, file_inode.i_uid);
            statbuf->st_gid = convert64(file_inode.osd2.linux2.l_i_gid_high, file_inode.i_gid);
            statbuf->st_rdev = 0;
            statbuf->st_size = convert64(file_inode.i_size_high, file_inode.i_size_lo);
            statbuf->st_blksize = sb.s_blocks_per_group;
            statbuf->st_blocks = convert64(file_inode.osd2.linux2.l_i_blocks_high, file_inode.i_blocks_lo);
            statbuf->st_atime = file_inode.i_atime;
            statbuf->st_mtime = file_inode.i_mtime;
            statbuf->st_ctime = file_inode.i_ctime;
            statbuf->st_crtime = file_inode.i_crtime;
            
            return 0;
        }

        memset(dir.name, '\0', EXT4_NAME_LEN);
        i += dir.rec_len;
    } while (i < 4096);

    return -2;
}

// Stat command
void cos_stat_command(int32_t argc, char argv[32][20])
{
    stat_t statbuf;
    char permissions[11];
    datetime_t dt;
    char time_buffer[20];
    const char *username, *groupname;
    int result;
    uint16_t major, minor;

    if (argc < 2) {
        cos_printf("stat: missing operand\n");
        cos_printf("Usage: stat <file>\n");
        return;
    }

    result = cos_stat(argv[1], &statbuf);
    
    if (result != 0) {
        if (result == -2) {
            cos_printf("stat: cannot stat '%s': No such file or directory\n", argv[1]);
        } else {
            cos_printf("stat: cannot stat '%s': Error %d\n", argv[1], result);
        }
        return;
    }

    major = (uint16_t)(statbuf.st_dev >> 8);
    minor = (uint16_t)(statbuf.st_dev & 0xFF);
    
    username = get_username(statbuf.st_uid);
    groupname = get_groupname(statbuf.st_gid);
    
    format_permissions(statbuf.st_mode, permissions);
    
    cos_printf("  File: %s\n", argv[1]);
    cos_printf("  Size: %d    Blocks: %d    IO Block: %d    ", 
               statbuf.st_size, statbuf.st_blocks, statbuf.st_blksize);
    
    if (statbuf.st_mode & 0x4000) {
        cos_printf("directory\n");
    } else if (statbuf.st_mode & 0x8000) {
        cos_printf("regular file\n");
    } else {
        cos_printf("unknown\n");
    }
    
    cos_printf("Device: %d,%d    Inode: %d    Links: %d\n",
               major, minor, statbuf.st_ino, statbuf.st_nlink);
    
    cos_printf("Access: (%d/%s)    Uid: (    %d/    %s)    Gid: (    %d/    %s)\n",
               statbuf.st_mode & 0777, permissions, statbuf.st_uid, username, 
               statbuf.st_gid, groupname);
    
    unix_to_datetime(statbuf.st_atime, &dt);
    format_datetime(&dt, time_buffer, sizeof(time_buffer));
    cos_printf("Access: %s\n", time_buffer);
    
    unix_to_datetime(statbuf.st_mtime, &dt);
    format_datetime(&dt, time_buffer, sizeof(time_buffer));
    cos_printf("Modify: %s\n", time_buffer);
    
    unix_to_datetime(statbuf.st_ctime, &dt);
    format_datetime(&dt, time_buffer, sizeof(time_buffer));
    cos_printf("Change: %s\n", time_buffer);
    
    unix_to_datetime(statbuf.st_crtime, &dt);
    format_datetime(&dt, time_buffer, sizeof(time_buffer));
    cos_printf("Birth: %s\n", time_buffer);
}

static bool check_for_valid_ext4_partition(void)
{
    superblock_t sb;

    memset(&sb, 0, sizeof(sb));
    read_superblock(&sb);
    if (sb.s_magic != SUPERBLOCK_MAGIC) {
        cos_printf("This disk image hasn't been formatted as an EXT4 filesystem.\n");
        return false;
    } else if (sb.s_log_block_size != 2) {
        cos_printf("This EXT4 filesystem isn't formatted as a 4096 block sized filesystem.\n");
        return false;
    } else if (sb.s_inode_size != DEFAULT_INODE_SIZE) {
        cos_printf("The EXT4 fileystem inode size is unsupported (not 256 bytes long).\n");
        return false;
    } else if ((((1024 << sb.s_log_block_size) * sb.s_blocks_per_group) / sb.s_inodes_per_group) != DEFAULT_INODE_RATIO) {
        cos_printf("The EXT4 filesystem isn't formatted with an inode ratio of 16384 per group.\n");
        return false;
    } else
        cos_printf("The EXT4 filesystem was correctly formatted and should be ready for use.\n");
    return true;
}

bool detect_ext4_partition_start_from_disk(void)
{
    uint8_t mbr[512] = {0};
    uint8_t gpt[512] = {0};

    read_byte((uint16_t *)mbr, 0, 1);

    if (*(uint16_t *)(&mbr[510]) == 0xAA55) {
        read_byte((uint16_t *)gpt, 1, 1);
        if (!cos_strncmp("EFI PART", (const char *)gpt, 8)) {
            cos_printf("GPT Partition Parsing hasn't yet been implemented.\n");
            cos_printf("You may shutdown your computer by pressing down the Power Button until the screen turns off.\n");
            return false;
        }
        cos_printf("This disk image is probably a real drive.\n");
        cos_printf("The MBR will now be parsed to locate a suitable EXT4 Partition.\n");
        for (uint8_t i = 0; i < 4; ++i) {
            if (((uint8_t *)mbr)[0x1BE + i * 16 + 4] == 0x83) {
                lba_start = *(uint32_t *)(&mbr[0x1BE + i * 16 + 8]);
                cos_printf("New Linux EXT4 Partition may have been found, lba_start is now: %d\n", lba_start);
                if (check_for_valid_ext4_partition()) {
                    cos_printf("A valid EXT4 Partition was found on your system.\n");
                    return true;
                }
            }
        }
    }
    if (check_for_valid_ext4_partition())
        return true;
    cos_printf("The drive or image you are trying to use doesn't comply with the current state of the project.\n");
    cos_printf("You may shutdown your computer by pressing the Power button until the screen turns off.\n");
    return false;
}

void normalize_path(const char* input_path, char* output_path)
{
    char * working_directory = get_env_path(); 
    char temp_path[256] = {0};
    char* segments[32] = {0}; // Maximum 32
    int segment_count = 0;
    
    if (input_path[0] != '/') {
        cos_strcpy(temp_path, working_directory);
        if (temp_path[cos_strlen(temp_path) - 1] != '/') {
            cos_strcat(temp_path, "/");
        }
        cos_strcat(temp_path, input_path);
    } else {
        cos_strcpy(temp_path, input_path);
    }
    
    char* token = cos_strtok(temp_path, "/");
    while (token != NULL && segment_count < 32) {
        if (cos_strcmp(token, ".") == 0) {
        } else if (cos_strcmp(token, "..") == 0) {
            if (segment_count > 0) {
                segment_count--;
            }
        } else {
            segments[segment_count] = token;
            segment_count++;
        }
        token = cos_strtok(NULL, "/");
    }
    
    if (segment_count == 0) {
        cos_strcpy(output_path, "/");
    } else {
        output_path[0] = '\0';
        for (int i = 0; i < segment_count; i++) {
            cos_strcat(output_path, "/");
            cos_strcat(output_path, segments[i]);
        }
    }
}

bool is_directory_accessible(const char* path)
{
    uint32_t dir_inode;
    superblock_t sb;
    block_descriptor_t block_group;
    inode_t metadata;
    uint64_t inode_block;
    uint64_t inode_offset;

    dir_inode = find_directory_inode(path, true, NULL);

    if (dir_inode == 0) {
        return false;
    }

    // Get data from inode
    read_superblock(&sb);
    inode_block = get_inode_block_group_number(&sb, dir_inode);
    inode_offset = get_inode_table_offset(&sb, dir_inode);
    read_block_descriptor_table(&block_group, inode_block);
    read_inode_metadata(inode_offset, &block_group, &metadata, CONTENT_NOT_ACCESSED, READ_MODE);

    // Test if its a directory
    if (!(metadata.i_mode & 0x4000)) {
        return false;
    }

    return true;
}

