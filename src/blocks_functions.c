#include "data_types.h"
#include "blocks_structures.h"
#include "ext4_functions.h"
#include <stdio.h>

static uint64_t table[256] =    {
                                    0x00000000, 0xf26b8303, 0xe13b70f7, 0x1350f3f4, 0xc79a971f, 0x35f1141c, 0x26a1e7e8, 0xd4ca64eb, 0x8ad958cf, 0x78b2dbcc, 0x6be22838, 0x9989ab3b, 0x4d43cfd0, 0xbf284cd3, 0xac78bf27, 0x5e133c24,
                                    0x105ec76f, 0xe235446c, 0xf165b798, 0x30e349b,  0xd7c45070, 0x25afd373, 0x36ff2087, 0xc494a384, 0x9a879fa0, 0x68ec1ca3, 0x7bbcef57, 0x89d76c54, 0x5d1d08bf, 0xaf768bbc, 0xbc267848, 0x4e4dfb4b,
                                    0x20bd8ede, 0xd2d60ddd, 0xc186fe29, 0x33ed7d2a, 0xe72719c1, 0x154c9ac2, 0x61c6936,  0xf477ea35, 0xaa64d611, 0x580f5512, 0x4b5fa6e6, 0xb93425e5, 0x6dfe410e, 0x9f95c20d, 0x8cc531f9, 0x7eaeb2fa,
                                    0x30e349b1, 0xc288cab2, 0xd1d83946, 0x23b3ba45, 0xf779deae, 0x5125dad,  0x1642ae59, 0xe4292d5a, 0xba3a117e, 0x4851927d, 0x5b016189, 0xa96ae28a, 0x7da08661, 0x8fcb0562, 0x9c9bf696, 0x6ef07595,
                                    0x417b1dbc, 0xb3109ebf, 0xa0406d4b, 0x522bee48, 0x86e18aa3, 0x748a09a0, 0x67dafa54, 0x95b17957, 0xcba24573, 0x39c9c670, 0x2a993584, 0xd8f2b687, 0xc38d26c,  0xfe53516f, 0xed03a29b, 0x1f682198,
                                    0x5125dad3, 0xa34e59d0, 0xb01eaa24, 0x42752927, 0x96bf4dcc, 0x64d4cecf, 0x77843d3b, 0x85efbe38, 0xdbfc821c, 0x2997011f, 0x3ac7f2eb, 0xc8ac71e8, 0x1c661503, 0xee0d9600, 0xfd5d65f4, 0xf36e6f7,
                                    0x61c69362, 0x93ad1061, 0x80fde395, 0x72966096, 0xa65c047d, 0x5437877e, 0x4767748a, 0xb50cf789, 0xeb1fcbad, 0x197448ae, 0xa24bb5a,  0xf84f3859, 0x2c855cb2, 0xdeeedfb1, 0xcdbe2c45, 0x3fd5af46,
                                    0x7198540d, 0x83f3d70e, 0x90a324fa, 0x62c8a7f9, 0xb602c312, 0x44694011, 0x5739b3e5, 0xa55230e6, 0xfb410cc2, 0x92a8fc1,  0x1a7a7c35, 0xe811ff36, 0x3cdb9bdd, 0xceb018de, 0xdde0eb2a, 0x2f8b6829,
                                    0x82f63b78, 0x709db87b, 0x63cd4b8f, 0x91a6c88c, 0x456cac67, 0xb7072f64, 0xa457dc90, 0x563c5f93, 0x82f63b7,  0xfa44e0b4, 0xe9141340, 0x1b7f9043, 0xcfb5f4a8, 0x3dde77ab, 0x2e8e845f, 0xdce5075c,
                                    0x92a8fc17, 0x60c37f14, 0x73938ce0, 0x81f80fe3, 0x55326b08, 0xa759e80b, 0xb4091bff, 0x466298fc, 0x1871a4d8, 0xea1a27db, 0xf94ad42f, 0xb21572c,  0xdfeb33c7, 0x2d80b0c4, 0x3ed04330, 0xccbbc033,
                                    0xa24bb5a6, 0x502036a5, 0x4370c551, 0xb11b4652, 0x65d122b9, 0x97baa1ba, 0x84ea524e, 0x7681d14d, 0x2892ed69, 0xdaf96e6a, 0xc9a99d9e, 0x3bc21e9d, 0xef087a76, 0x1d63f975, 0xe330a81,  0xfc588982,
                                    0xb21572c9, 0x407ef1ca, 0x532e023e, 0xa145813d, 0x758fe5d6, 0x87e466d5, 0x94b49521, 0x66df1622, 0x38cc2a06, 0xcaa7a905, 0xd9f75af1, 0x2b9cd9f2, 0xff56bd19, 0xd3d3e1a,  0x1e6dcdee, 0xec064eed,
                                    0xc38d26c4, 0x31e6a5c7, 0x22b65633, 0xd0ddd530, 0x417b1db,  0xf67c32d8, 0xe52cc12c, 0x1747422f, 0x49547e0b, 0xbb3ffd08, 0xa86f0efc, 0x5a048dff, 0x8ecee914, 0x7ca56a17, 0x6ff599e3, 0x9d9e1ae0,
                                    0xd3d3e1ab, 0x21b862a8, 0x32e8915c, 0xc083125f, 0x144976b4, 0xe622f5b7, 0xf5720643, 0x7198540,  0x590ab964, 0xab613a67, 0xb831c993, 0x4a5a4a90, 0x9e902e7b, 0x6cfbad78, 0x7fab5e8c, 0x8dc0dd8f,
                                    0xe330a81a, 0x115b2b19, 0x20bd8ed,  0xf0605bee, 0x24aa3f05, 0xd6c1bc06, 0xc5914ff2, 0x37faccf1, 0x69e9f0d5, 0x9b8273d6, 0x88d28022, 0x7ab90321, 0xae7367ca, 0x5c18e4c9, 0x4f48173d, 0xbd23943e,
                                    0xf36e6f75, 0x105ec76,  0x12551f82, 0xe03e9c81, 0x34f4f86a, 0xc69f7b69, 0xd5cf889d, 0x27a40b9e, 0x79b737ba, 0x8bdcb4b9, 0x988c474d, 0x6ae7c44e, 0xbe2da0a5, 0x4c4623a6, 0x5f16d052, 0xad7d5351
                                };

uint32_t cos_generate_crc32c_checksum(uint8_t *bitstream, uint32_t old_checksum, uint32_t data_length)
{
    while (data_length--)
        old_checksum = table[(old_checksum ^ *bitstream++) & 0xFF] ^ (old_checksum >> 8);
    return ~old_checksum;
}
/*
size_t little_endian_reversal(size_t value, size_t data_size)
{
    switch (data_size) {
    case 2:
        return ((value >> 8) & 0x00FF) |
               ((value << 8) & 0xFF00);
        break;
    case 4:
        return ((value >> 24) & 0x000000FF) |
               ((value >> 8) & 0x0000FF00) |
               ((value << 8) & 0x00FF0000) |
               ((value << 24) & 0xFF000000);
        break;
    case 8 :
        return ((0x00000000000000FF & value) << 56) |
               ((0x000000000000FF00 & value) << 40) |
               ((0x0000000000FF0000 & value) << 24) |
               ((0x00000000FF000000 & value) << 8) |
               ((0x000000FF00000000 & value) >> 8) |
               ((0x0000FF0000000000 & value) >> 24) |
               ((0x00FF000000000000 & value) >> 40) |
               ((0xFF00000000000000 & value) >> 56);
    default:
        return value;
    }
    return value;
}
*/
/*System Calls à remplacer par les fonctions ASM.*/
void write_superblock_to_disk(char *disk_address, size_t filesystem_size, size_t *offset, uint64_t timestamp)
{
    superblock_t superblock_header;

    uint8_t block_bitstream[1025] = {0};
    cos_memcpy(&block_bitstream[0], (uint8_t[16]){DEFAULT_UUID_VOLUME}, 16);

    uint32_t number_of_blocks_per_group = 8 * DEFAULT_BLOCK_SIZE;
    size_t total_number_of_inodes = filesystem_size / DEFAULT_INODE_RATIO;
    size_t number_of_block_groups = filesystem_size / (number_of_blocks_per_group * DEFAULT_BLOCK_SIZE);
    uint32_t number_of_inodes_per_group = total_number_of_inodes / number_of_block_groups;
    size_t total_number_of_blocks = number_of_block_groups * number_of_blocks_per_group;
    size_t number_of_blocks_required_to_store_inodes = (DEFAULT_INODE_SIZE * total_number_of_inodes) / DEFAULT_BLOCK_SIZE;

    *offset = 0x400;

    superblock_header.s_inodes_count = filesystem_size / DEFAULT_INODE_RATIO;
    superblock_header.s_blocks_count_lo = filesystem_size / DEFAULT_BLOCK_SIZE;
    superblock_header.s_r_blocks_count_lo = 0;
    //Must be updated after inodes have been allocated.
    superblock_header.s_free_blocks_count_lo = (total_number_of_blocks - number_of_blocks_required_to_store_inodes - 2 * number_of_block_groups - 2 - 5) & 0xFFFFFFFF;
    superblock_header.s_free_inodes_count = total_number_of_inodes - 11; /* TBD if journal and other unimplemented functionalities are made */
    superblock_header.s_first_data_block = 0;
    superblock_header.s_log_block_size = 2; /* Needs to return the number by which the bitshift of 1024 returns the block size used */
    superblock_header.s_blocks_per_group = number_of_blocks_per_group;
    superblock_header.s_inodes_per_group = number_of_inodes_per_group;
    if (BIGALLOC_FLAG) {
        superblock_header.s_log_cluster_size = DEFAULT_LOG_CLUSTER_SIZE;
        superblock_header.s_clusters_per_group = DEFAULT_CLUSTERS_PER_GROUP_NUMBER;
    } else {
        superblock_header.s_log_cluster_size = superblock_header.s_log_block_size;
        superblock_header.s_clusters_per_group = superblock_header.s_blocks_per_group;
    }
    superblock_header.s_mtime = 0;
    superblock_header.s_wtime = timestamp;
    superblock_header.s_lastcheck = timestamp;
    superblock_header.s_mkfs_time = timestamp;
    superblock_header.s_mnt_count = 0;
    superblock_header.s_max_mnt_count = 0xFFFF;
    superblock_header.s_magic = SUPERBLOCK_MAGIC;
    superblock_header.s_state = 1;
    superblock_header.s_errors = 1;
    superblock_header.s_minor_rev_level = 0;
    //Lastcheck is always at least equals to s_wtime.
    superblock_header.s_checkinterval = DEFAULT_TIME_CHECKS;
    superblock_header.s_creator_os = 0;
    superblock_header.s_rev_level = 1;
    superblock_header.s_def_resuid = DEFAULT_UID_RESERVED;
    superblock_header.s_def_resgid = DEFAULT_GID_RESERVED;

    superblock_header.s_first_ino = 11;
    superblock_header.s_inode_size = DEFAULT_INODE_SIZE;
    superblock_header.s_block_group_nr = 0;
    superblock_header.s_feature_compat = 0;
    superblock_header.s_feature_incompat = 0x40 | 0x2; // Files in the filesystem make use of extents.
    superblock_header.s_feature_ro_compat = 0;
    cos_memcpy(superblock_header.s_uuid, (uint8_t[16]){DEFAULT_UUID_VOLUME}, sizeof(uint8_t) * 16);
    cos_memcpy(superblock_header.s_volume_name, (char[16]){0}, sizeof(char) * 16);
    cos_memcpy(superblock_header.s_last_mounted, (char[64]){0}, sizeof(char) * 64);
    superblock_header.s_algorithm_usage_bitmap = 0;

    superblock_header.s_prealloc_blocks = 0;
    superblock_header.s_prealloc_dir_blocks = 0;
    superblock_header.s_reserved_gdt_blocks = 0;

    //Useless for now
    cos_memcpy(superblock_header.s_journal_uuid, (uint8_t[16]){0}, sizeof(uint8_t) * 64);
    superblock_header.s_journal_inum = 0;   // If the Journalisation is added, change the 0 by 8;
    superblock_header.s_journal_dev = 0;
    superblock_header.s_last_orphan = 0;
    #define IDOIT sizeof(inode_t)

    cos_memcpy(superblock_header.s_hash_seed, (uint32_t[4]){0}, sizeof(uint32_t) * 4);
    superblock_header.s_def_hash_version = 0;
    superblock_header.s_jnl_backup_type = 0;
    superblock_header.s_desc_size = BLOCK_DESCRIPTOR_SIZE;
    superblock_header.s_default_mount_opts = 0;
    superblock_header.s_first_meta_bg = 0;
    cos_memcpy(superblock_header.s_jnl_blocks, (uint32_t[17]){0}, sizeof(uint32_t) * 17);

    //Useless for now
    superblock_header.s_blocks_count_hi = (uint32_t)(((uint64_t)(filesystem_size / DEFAULT_BLOCK_SIZE)) >> 4);
    superblock_header.s_r_blocks_count_hi = 0;
    superblock_header.s_free_blocks_count_hi = superblock_header.s_blocks_count_hi;
    superblock_header.s_min_extra_isize = 32;
    superblock_header.s_want_extra_isize = 32;
    superblock_header.s_flags = 1;
    superblock_header.s_raid_stride = 0;
    superblock_header.s_mmp_update_interval = 0;
    superblock_header.s_mmp_block = MMP_BLOCK_INDEX;
    superblock_header.s_raid_stripe_width = 0;
    superblock_header.s_log_groups_per_flex = DEFAULT_LOG_GROUP_PER_FLEX_SIZE;
    superblock_header.s_checksum_type = 1;
    superblock_header.s_encryption_level = 0;
    superblock_header.s_reserved_pad = 0;
    superblock_header.s_kbytes_written = 0;
    superblock_header.s_snapshot_inum = 0;
    superblock_header.s_snapshot_id = 0;
    superblock_header.s_snapshot_r_blocks_count = 0;
    superblock_header.s_snapshot_list = 0;
    superblock_header.s_error_count = 0;
    superblock_header.s_first_error_time = 0;
    superblock_header.s_first_error_ino = 0;
    superblock_header.s_first_error_block = 0;
    cos_memcpy(superblock_header.s_first_error_func, (uint8_t[32]){0}, sizeof(uint8_t) * 32);
    superblock_header.s_first_error_line = 0;
    superblock_header.s_last_error_time = 0;
    superblock_header.s_last_error_ino = 0;
    superblock_header.s_last_error_line = 0;
    superblock_header.s_last_error_block = 0;
    cos_memcpy(superblock_header.s_last_error_func, (uint8_t[32]){0}, sizeof(uint8_t) * 32);
    cos_memcpy(superblock_header.s_mount_opts, (uint8_t[64]){0}, sizeof(uint8_t) * 64);
    superblock_header.s_usr_quota_inum = 3;
    superblock_header.s_grp_quota_inum = 4;
    superblock_header.s_overhead_clusters = 0;
    cos_memcpy(superblock_header.s_backup_bgs, (uint32_t[2]){0}, sizeof(uint32_t) * 2);
    cos_memcpy(superblock_header.s_encrypt_algos, (uint8_t[4]){0}, sizeof(uint8_t) * 4);
    cos_memcpy(superblock_header.s_encrypt_pw_salt, (uint8_t[16]){0}, sizeof(uint8_t) * 16);
    superblock_header.s_lpf_ino = 11;
    superblock_header.s_prj_quota_inum = 0;
    superblock_header.s_checksum_seed = cos_generate_crc32c_checksum(block_bitstream, 0, 16);
    superblock_header.s_wtime_hi = (uint8_t)(timestamp >> 32);
    superblock_header.s_mtime_hi = (uint8_t)(timestamp >> 32);
    superblock_header.s_mkfs_time_hi = (uint8_t)(timestamp >> 32);
    superblock_header.s_lastcheck_hi = (uint8_t)(timestamp >> 32);
    superblock_header.s_first_error_time_hi = (uint8_t)(timestamp >> 32);
    superblock_header.s_last_error_time_hi = (uint8_t)(timestamp >> 32);
    superblock_header.s_first_error_errcode = 0;
    superblock_header.s_last_error_errcode = 0;
    superblock_header.s_encoding = 0;
    superblock_header.s_encoding_flags = 0;
    superblock_header.s_orphan_file_inum = 0;
    cos_memcpy(superblock_header.s_reserved, (uint32_t[94]){0}, sizeof(uint32_t) * 94);
    cos_memcpy(&block_bitstream[0], &superblock_header, 1024);
    superblock_header.s_checksum = cos_generate_crc32c_checksum(block_bitstream, 0, 1024);

    cos_memcpy(&disk_address[*offset], &superblock_header, SUPERBLOCK_SIZE); /*Replace by an ASM Function that write in a HardDrive*/
}

void write_block_descriptor(char *disk_address, size_t filesystem_size, size_t *offset)
{
    uint32_t number_of_blocks_per_group = 8 * DEFAULT_BLOCK_SIZE;
    size_t total_number_of_inodes = filesystem_size / DEFAULT_INODE_RATIO;
    size_t number_of_block_groups = filesystem_size / (number_of_blocks_per_group * DEFAULT_BLOCK_SIZE);
    uint32_t number_of_inodes_per_group = total_number_of_inodes / number_of_block_groups;
    size_t number_of_blocks_required_to_store_inodes = (DEFAULT_INODE_SIZE * total_number_of_inodes) / DEFAULT_BLOCK_SIZE;

    size_t used_blocks = number_of_blocks_required_to_store_inodes + 2 * number_of_block_groups + 2 + 5;
    size_t used_inodes = 11; /* At least 11 are reserved by the EXT4 filesystem by design and inode 0 does not exist.*/
    size_t count = 2;
    //size_t total_free_block_count = number_of_block_groups * number_of_blocks_per_group - number_of_block_groups * 2 - number_of_blocks_required_to_store_inodes;
    //size_t total_free_inode_count = total_number_of_inodes;

    *offset = DEFAULT_BLOCK_SIZE;

    uint8_t bitmap_bitstream[16 + DEFAULT_BLOCK_SIZE] = {0};
    cos_memcpy(&bitmap_bitstream[0], (uint8_t[16]){DEFAULT_UUID_VOLUME}, 16);

    uint8_t block_bitstream[84] = {0};
    cos_memcpy(&block_bitstream[0], (uint8_t[16]){DEFAULT_UUID_VOLUME}, 16);

    block_descriptor_t desc;
    block_descriptor_t temp_desc_cpy;
    uint32_t checksum = 0;
    for (size_t i = 0; i < number_of_block_groups; i += BLOCK_DESCRIPTOR_SIZE, ++count) {

        desc.bg_block_bitmap_lo = (uint32_t)(count & 0xFFFFFFFF);
        desc.bg_inode_bitmap_lo = (uint32_t)((count + number_of_block_groups) & 0xFFFFFFFF);
        desc.bg_inode_table_lo = (uint32_t)((count + 2 * number_of_block_groups) & 0xFFFFFFFF);

        desc.bg_free_blocks_count_lo = (count == 2) ? (uint16_t)((number_of_blocks_per_group - used_blocks) & 0xFFFF) : (uint16_t)(number_of_blocks_per_group & 0xFFFF);
        desc.bg_free_inodes_count_lo = (count == 2) ? (uint16_t)((number_of_inodes_per_group - 11) & 0xFFFF) : (uint16_t)(number_of_inodes_per_group & 0xFFFF);
        desc.bg_used_dirs_count_lo = 2; /*By default, inode 2 is meant to be root and 11 is lost+found*/
        desc.bg_flags = 0;
        desc.bg_exclude_bitmap_lo = 0; /*TBD*/
        desc.bg_itable_unused_lo = 0; /*If an unused count is set, the system treats it as unavailable space instead of free space.*/

        desc.bg_block_bitmap_hi = (uint32_t)(count >> 32);
        desc.bg_inode_bitmap_hi = (uint32_t)((count + number_of_block_groups) >> 32);
        desc.bg_inode_table_hi = (uint32_t)((count + 2 * number_of_block_groups) >> 32);
        desc.bg_free_blocks_count_hi = (uint16_t)(number_of_blocks_per_group >> 16);
        desc.bg_free_inodes_count_hi = (count == 2) ? (uint16_t)((number_of_inodes_per_group - 12) >> 16) : (uint16_t)(number_of_inodes_per_group >> 16);
        desc.bg_used_dirs_count_hi = 0; /*This is to be updated when a new inode is added.*/
        desc.bg_itable_unused_hi = (uint16_t)0; /*This value shouldn't be set for the same reason.*/
        desc.bg_exclude_bitmap_hi = 0;
        desc.bg_reserved = 0;

        temp_desc_cpy = desc;
        cos_memcpy(&block_bitstream[16], (uint8_t[4]){(count >> 24) & 0xFF, (count >> 16) & 0xFF, (count >> 8) & 0xFF, count & 0xFF}, 4);
        cos_memcpy(&block_bitstream[20], &temp_desc_cpy, 64);
        desc.bg_checksum = cos_generate_crc32c_checksum(block_bitstream, 0, 84) & 0xFFFF;

        cos_memcpy(&bitmap_bitstream[16], &disk_address[count * DEFAULT_BLOCK_SIZE], DEFAULT_BLOCK_SIZE);
        checksum = cos_generate_crc32c_checksum(bitmap_bitstream, 0, DEFAULT_BLOCK_SIZE + 16);
        desc.bg_block_bitmap_csum_lo = (checksum & 0xFFFF);
        desc.bg_block_bitmap_csum_hi = (checksum >> 16) & 0xFFFF;

        cos_memcpy(&bitmap_bitstream[16], &disk_address[(count + number_of_block_groups) * DEFAULT_BLOCK_SIZE], DEFAULT_BLOCK_SIZE);
        checksum = cos_generate_crc32c_checksum(bitmap_bitstream, 0, DEFAULT_BLOCK_SIZE + 16);
        desc.bg_inode_bitmap_csum_lo = checksum & 0xFFFF;
        desc.bg_inode_bitmap_csum_hi = (checksum >> 16) & 0xFFFF;

        cos_memcpy(&disk_address[*offset], &desc, BLOCK_DESCRIPTOR_SIZE);
        *offset += BLOCK_DESCRIPTOR_SIZE;
    }

    *offset = 2 * DEFAULT_BLOCK_SIZE;

    //Adds the block bitmap to the disk.
    //The number of used_blocks is divided by 8 and then the remainder is taken for the final byte.
    count = (used_blocks >> 3) + *offset;
    for (size_t i = *offset; i < count; ++i)
        disk_address[i] = (char)0xFF;
    disk_address[(used_blocks >> 3) + *offset] = ~(0xFF << (used_blocks % 8));

    if (number_of_blocks_per_group < DEFAULT_BLOCK_SIZE * 8)
        memset(&disk_address[*offset + number_of_blocks_per_group / 8], (uint8_t)0xFF, DEFAULT_BLOCK_SIZE - number_of_blocks_per_group / 8);
    *offset += number_of_block_groups * DEFAULT_BLOCK_SIZE;

    //Adds the inode bitmap to the disk.
    //The number of used_inodes is divided by 8 and then the remainder is taken for the final byte.

    count = (used_inodes >> 3) + *offset;
    for (size_t i = *offset; i < count; ++i)
        disk_address[i] = (char)0xFF;
    disk_address[(used_inodes >> 3) + *offset] = ~(0xFF << (used_inodes % 8));

    if (number_of_inodes_per_group < DEFAULT_BLOCK_SIZE * 8)
        memset(&disk_address[*offset + number_of_inodes_per_group / 8], (uint8_t)0xFF, DEFAULT_BLOCK_SIZE - number_of_inodes_per_group / 8);
    *offset += number_of_block_groups * DEFAULT_BLOCK_SIZE;
}

void write_inode_to_disk(char *disk_address, size_t filesystem_size, size_t *offset, uint64_t timestamp)
{
    uint32_t number_of_blocks_per_group = 8 * DEFAULT_BLOCK_SIZE;
    size_t total_number_of_inodes = filesystem_size / DEFAULT_INODE_RATIO;
    size_t number_of_block_groups = filesystem_size / (number_of_blocks_per_group * DEFAULT_BLOCK_SIZE);
    size_t number_of_blocks_required_to_store_inodes = (DEFAULT_INODE_SIZE * total_number_of_inodes) / DEFAULT_BLOCK_SIZE;
    size_t used_blocks = number_of_blocks_required_to_store_inodes + 2 * number_of_block_groups + 2;
    size_t number_sector = 0;

    uint8_t checksum_bitstream[280] = {0};
    cos_memcpy(&checksum_bitstream[0], (uint8_t[16]){DEFAULT_UUID_VOLUME}, 16);

    extent_head_t head;
    head.eh_magic = EXTENT_MAGIC;
    head.eh_entries = 1;
    head.eh_max = 4; /*Max number of entries following the header. While it is 1, there can be at most 4 entries because there's 60 available bytes.*/
    head.eh_depth = 0; /*We only add leaf nodes for this PoC. TODO: Implement complete extent tree support.*/
    head.eh_generation = 0;

    inode_t inode;
    extent_leaf_t leaf;
    uint32_t checksum;
    for (size_t i = 1; i < 12; ++i) {

        if (i == 2 || i == 11) {
            inode.i_mode = 0x4000 | 0x800 | 0x400 | 0x100 | 0x80 | 0x40 | 0x20 | 0x8 | 0x4 | 0x1;
            inode.i_flags = 0x80000;
            inode.i_links_count = 2;

            if (i == 2) {
                ++inode.i_links_count;
                leaf.ee_block = 0;
                leaf.ee_len = 1;
                leaf.ee_start_lo = used_blocks & 0xFFFFFFFF;
                leaf.ee_start_hi = (used_blocks >> 32) & 0xFFFF;
            } else {
                leaf.ee_block = 0;
                leaf.ee_len = 4;
                leaf.ee_start_lo = (used_blocks + 1) & 0xFFFFFFFF;
                leaf.ee_start_hi = ((used_blocks + 1) >> 32) & 0xFFFF;
            }

            number_sector = (leaf.ee_len * 4096) / 512;

            //These fields represent the number of 512 byte blocks used by the file.
            inode.i_blocks_lo = (uint32_t)(number_sector & 0xFFFFFFFF);
            inode.osd2.linux2.l_i_blocks_high = (uint16_t)((number_sector >> 32) & 0xFFFF);
            cos_memcpy(&inode.i_block[0], &head, sizeof(uint32_t) * 3);
            cos_memcpy(&inode.i_block[3], &leaf, sizeof(uint32_t) * 3);

            //These fields represent the file size in bytes.
            inode.i_size_lo = (uint32_t)((512 * number_sector) & 0xFFFFFFFF);
            inode.i_size_high = (uint32_t)(((512 * number_sector) >> 32) & 0xFFFFFFFF);

            inode.i_extra_isize = 32;
            inode.i_generation = head.eh_generation;

        } else {
            inode.i_mode = 0;
            inode.i_flags = 0;
            inode.i_links_count = 0;

            inode.i_blocks_lo = 0;
            inode.osd2.linux2.l_i_blocks_high = 0;
            cos_memcpy(inode.i_block, (uint32_t[15]){0}, sizeof(uint32_t) * 15);

            inode.i_size_lo = 0;
            inode.i_size_high = 0;

            inode.i_extra_isize = 0;
            inode.i_generation = 0;
        }
        inode.i_gid = (uint16_t)0;
        inode.osd2.linux2.l_i_gid_high = (uint16_t)0;
        inode.i_uid = (uint16_t)0;
        inode.osd2.linux2.l_i_uid_high = (uint16_t)0;

        //Time in seconds since the Epoch
        inode.i_atime = timestamp;
        inode.i_ctime = timestamp;
        inode.i_mtime = timestamp;
        inode.i_crtime = timestamp;

        //Sub-second precision.
        inode.i_ctime_extra = 0;
        inode.i_mtime_extra = 0;
        inode.i_atime_extra = 0;
        inode.i_crtime_extra = 0;

        //Deletion time.
        inode.i_dtime = 0;

        //Useless for us.
        inode.i_file_acl_lo = (uint32_t) 0;
        inode.osd2.linux2.l_i_file_acl_high = (uint16_t) 0;

        inode.osd2.linux2.l_i_reserved = 0;
        inode.i_obso_faddr = 0;
        inode.osd1.linux1.l_i_version = (uint32_t)0;
        inode.i_version_hi = (uint32_t)0;
        inode.i_projid = 0;
        memset(inode.i_cos_reserved, '\0', sizeof(uint8_t) * 96);

        cos_memcpy(&checksum_bitstream[16], (uint8_t[4]){(i >> 24) & 0xFF, (i >> 16) & 0xFF, (i >> 8) & 0xFF, i & 0xFF}, 4);
        cos_memcpy(&checksum_bitstream[20], (uint8_t[4]){(inode.i_generation >> 24) & 0xFF, (inode.i_generation >> 16) & 0xFF, (inode.i_generation >> 8) & 0xFF, inode.i_generation & 0xFF}, 4);
        cos_memcpy(&checksum_bitstream[24], &disk_address[*offset], DEFAULT_INODE_SIZE);
        checksum = cos_generate_crc32c_checksum(checksum_bitstream, 0, 280);
        inode.osd2.linux2.l_i_checksum_lo = (uint16_t)(checksum & 0xFFFF);
        inode.i_checksum_hi = (uint16_t)((checksum >> 16) & 0xFFFF);

        cos_memcpy(&disk_address[*offset], &inode, DEFAULT_INODE_SIZE);
        *offset += DEFAULT_INODE_SIZE;
    }

    dirent2_t dir_entry;
    dir_entry.inode = 2;
    dir_entry.rec_len = 12;
    dir_entry.name_len = 1;
    dir_entry.file_type = 2;
    cos_memcpy(dir_entry.name, ".", 1);
    cos_memcpy(&disk_address[used_blocks * 4096], &dir_entry, 12);

    dir_entry.inode = 11;
    cos_memcpy(&disk_address[(used_blocks + 1) * 4096], &dir_entry, 12);

    dir_entry.inode = 2;
    dir_entry.rec_len = 12;
    dir_entry.name_len = 2;
    dir_entry.file_type = 2;
    cos_memcpy(dir_entry.name, "..", 2);
    cos_memcpy(&disk_address[used_blocks * 4096 + 12], &dir_entry, 12);

    dir_entry.rec_len = DEFAULT_BLOCK_SIZE - 24;
    cos_memcpy(&disk_address[(used_blocks + 1) * 4096 + 12], &dir_entry, 12);

    dir_entry.inode = 11;
    dir_entry.rec_len = DEFAULT_BLOCK_SIZE - 36;
    dir_entry.name_len = 10;
    dir_entry.file_type = 2;
    cos_memcpy(dir_entry.name, "lost+found", 10);
    cos_memcpy(&disk_address[used_blocks * 4096 + 24], &dir_entry, 20);

    uint8_t dir_bitstream[16 + DEFAULT_BLOCK_SIZE] = {0};
    cos_memcpy(&dir_bitstream[0], (uint8_t[16]){DEFAULT_UUID_VOLUME}, 16);

    dirent2_tail_t tail;
    tail.det_rec_len = 12;
    tail.det_reserved_zero = 0;
    tail.det_reserved_zero2 = 0;
    tail.det_reserved_ft = 0xDE;

    cos_memcpy(&dir_bitstream[16], &disk_address[used_blocks * 4096], DEFAULT_BLOCK_SIZE);
    tail.det_checksum = cos_generate_crc32c_checksum(dir_bitstream, 0, DEFAULT_BLOCK_SIZE + 16);
    cos_memcpy(&disk_address[used_blocks * 4096 + 4084], &tail, 12);

    cos_memcpy(&dir_bitstream[16], &disk_address[(used_blocks + 1) * 4096], DEFAULT_BLOCK_SIZE);
    tail.det_checksum = cos_generate_crc32c_checksum(dir_bitstream, 0, DEFAULT_BLOCK_SIZE + 16);
    cos_memcpy(&disk_address[(used_blocks + 1) * 4096 + 4084], &tail, 12);

    dir_entry.inode = 0;
    dir_entry.rec_len = DEFAULT_BLOCK_SIZE;
    dir_entry.name_len = 0;
    dir_entry.file_type = 0;
    memset(dir_entry.name, 0, 255);

    //Ext4 filesystem mount requires ALL yet to be used directory blocks to start with a dirent structure that is NULL except for the rec_len field which MUST be equal to block size.
    cos_memcpy(&disk_address[(used_blocks + 2) * 4096], &dir_entry, sizeof(dirent2_t));
    cos_memcpy(&disk_address[(used_blocks + 3) * 4096], &dir_entry, sizeof(dirent2_t));
    cos_memcpy(&disk_address[(used_blocks + 4) * 4096], &dir_entry, sizeof(dirent2_t));
}
