#pragma once

#include "data_types.h"

/*
    Inode number = filesystem_size / 16384
    Block Number = 8 * block_size

    Default Block Size = 4096;
*/

#define DEFAULT_BLOCK_SIZE 					4096U
#define DEFAULT_BLOCK_GROUP					8
#define DEFAULT_INODE_SIZE 					256U
#define DEFAULT_INODE_RATIO 				16384U

#define DEFAULT_TOTAL_INODE_COUNT			65536U //Valeur issue du schéma de ce site https://blogs.oracle.com/linux/post/understanding-ext4-disk-layout-part-1
#define DEFAULT_TOTAL_BLOCK_COUNT			262144U 
#define DEFAULT_FREE_BLOCK_COUNT			249189U
#define DEFAULT_FREE_INODE_COUNT			65525U
#define DEFAULT_BLOCK_GROUP_NUMBER			32768U
#define DEFAULT_INODE_GROUP_NUMBER			8192U
#define DEFAULT_UUID_VOLUME					0x0f, 0x63, 0x64,  0x99,  0x7a, 0x63,0x40, 0x77, 0x8d, 0xc5,0x45, 0xf4, 0xac,0xc3, 0x1f, 0x7c
#define DEFAULT_SUPERBLOCK_CHECKSUM			0x0e51d7e1U

#define DEFAULT_LOG_BLOCK_SIZE 				1024U
#define DEFAULT_LOG_CLUSTER_SIZE 			1024U
#define DEFAULT_CLUSTERS_PER_GROUP_NUMBER 	0U /*Valeur par défaut à déterminer*/
#define DEFAULT_INODES_PER_GROUP_NUMBER 	0U /*Valeur par défaut à déterminer*/
#define BIGALLOC_FLAG 						0U
#define DEFAULT_TIME_CHECKS 				0U
#define DEFAULT_UID_RESERVED 				0U
#define DEFAULT_GID_RESERVED 				0U
#define SUPERBLOCK_MAGIC					0xEF53U
#define MMP_BLOCK_INDEX						0U
#define DEFAULT_LOG_GROUP_PER_FLEX_SIZE 	0U

#define EXTENT_MAGIC						0xF30AU
#define EXT4_NAME_LEN						255U

typedef struct superblock {

    uint32_t	s_inodes_count;
	uint32_t	s_blocks_count_lo;
	uint32_t	s_r_blocks_count_lo;
	uint32_t	s_free_blocks_count_lo;
    uint32_t	s_free_inodes_count;
	uint32_t	s_first_data_block;
	uint32_t	s_log_block_size;
	uint32_t	s_log_cluster_size;
    uint32_t	s_blocks_per_group;
	uint32_t	s_clusters_per_group;
	uint32_t	s_inodes_per_group;
	uint32_t	s_mtime;
    uint32_t	s_wtime;
	uint16_t	s_mnt_count;
	uint16_t	s_max_mnt_count;
	uint16_t	s_magic;
	uint16_t	s_state;
	uint16_t	s_errors;
	uint16_t	s_minor_rev_level;
    uint32_t	s_lastcheck;
	uint32_t	s_checkinterval;
	uint32_t	s_creator_os;
	uint32_t	s_rev_level;
    uint16_t	s_def_resuid;
	uint16_t	s_def_resgid;

	/*
	 * These fields are for EXT4_DYNAMIC_REV superblocks only.
	 *
	 * Note: the difference between the compatible feature set and
	 * the incompatible feature set is that if there is a bit set
	 * in the incompatible feature set that the kernel doesn't
	 * know about, it should refuse to mount the filesystem.
	 *
	 * e2fsck's requirements are more strict; if it doesn't know
	 * about a feature in either the compatible or incompatible
	 * feature set, it must abort and not try to meddle with
	 * things it doesn't understand...
	 */
	uint32_t	s_first_ino;
	uint16_t    s_inode_size;
	uint16_t	s_block_group_nr;
	uint32_t	s_feature_compat;
    uint32_t	s_feature_incompat;
	uint32_t	s_feature_ro_compat;
    uint8_t	    s_uuid[16];
    char	    s_volume_name[16];
    char	    s_last_mounted[64];	/* nonstring */
    uint32_t	s_algorithm_usage_bitmap;

	/*
	 * Performance hints.  Directory preallocation should only
	 * happen if the EXT4_FEATURE_COMPAT_DIR_PREALLOC flag is on.
	 */
	uint8_t	    s_prealloc_blocks;
	uint8_t	    s_prealloc_dir_blocks;
	uint16_t	s_reserved_gdt_blocks;

	/*
	 * Journaling support valid if EXT4_FEATURE_COMPAT_HAS_JOURNAL set.
	 */
    uint8_t	    s_journal_uuid[16];
    uint32_t	s_journal_inum;
	uint32_t	s_journal_dev;
	uint32_t	s_last_orphan;
	uint32_t	s_hash_seed[4];
	uint8_t	    s_def_hash_version;
	uint8_t	    s_jnl_backup_type;
	uint16_t    s_desc_size;
    uint32_t	s_default_mount_opts;
	uint32_t	s_first_meta_bg;
	uint32_t	s_mkfs_time;
	uint32_t	s_jnl_blocks[17];

	/* 64bit support valid if EXT4_FEATURE_INC__leOMPAT_64BIT */
    uint32_t	s_blocks_count_hi;
	uint32_t	s_r_blocks_count_hi;
	uint32_t	s_free_blocks_count_hi;
	uint16_t	s_min_extra_isize;
	uint16_t	s_want_extra_isize;
	uint32_t	s_flags;
	uint16_t    s_raid_stride;
	uint16_t    s_mmp_update_interval; /*Seemingly unimplemented*/
	uint64_t    s_mmp_block;
	uint32_t    s_raid_stripe_width;
	uint8_t	    s_log_groups_per_flex;
	uint8_t	    s_checksum_type;
	uint8_t	    s_encryption_level;
	uint8_t	    s_reserved_pad;
	uint64_t	s_kbytes_written;
	uint32_t	s_snapshot_inum; /*TBD*/
	uint32_t	s_snapshot_id;
	uint64_t	s_snapshot_r_blocks_count;
	uint32_t	s_snapshot_list; /*TBD*/
	uint32_t	s_error_count;
	uint32_t	s_first_error_time;
	uint32_t	s_first_error_ino;
	uint64_t	s_first_error_block;
	uint8_t	    s_first_error_func[32]; /* nonstring */
	uint32_t	s_first_error_line;
	uint32_t	s_last_error_time;
	uint32_t	s_last_error_ino;
	uint32_t	s_last_error_line;
	uint64_t	s_last_error_block;
	uint8_t	    s_last_error_func[32];	/* nonstring */
	uint8_t	    s_mount_opts[64];
	uint32_t	s_usr_quota_inum; /*TBD*/
	uint32_t	s_grp_quota_inum; /*TBD*/
	uint32_t	s_overhead_clusters;
	uint32_t	s_backup_bgs[2];
	uint8_t	    s_encrypt_algos[4];
	uint8_t	    s_encrypt_pw_salt[16];
	uint32_t	s_lpf_ino; /*TBD*/
	uint32_t	s_prj_quota_inum; /*TBD*/
	uint32_t	s_checksum_seed; /*TBD*/
	uint8_t	    s_wtime_hi;
	uint8_t	    s_mtime_hi;
	uint8_t	    s_mkfs_time_hi;
	uint8_t	    s_lastcheck_hi;
	uint8_t	    s_first_error_time_hi;
	uint8_t	    s_last_error_time_hi;
	uint8_t	    s_first_error_errcode;
	uint8_t     s_last_error_errcode;
	uint16_t    s_encoding;
	uint16_t    s_encoding_flags;
	uint32_t    s_orphan_file_inum; /*TBD*/
	uint32_t	s_reserved[94];
	uint32_t	s_checksum; /*TBD*/
} superblock_t;

#define SUPERBLOCK_SIZE sizeof(superblock_t)

typedef struct block_descriptor {
    uint32_t  bg_block_bitmap_lo;
    uint32_t  bg_inode_bitmap_lo;
    uint32_t  bg_inode_table_lo;
    uint16_t  bg_free_blocks_count_lo;
    uint16_t  bg_free_inodes_count_lo;
    uint16_t  bg_used_dirs_count_lo;
    uint16_t  bg_flags;
    uint32_t  bg_exclude_bitmap_lo;
    uint16_t  bg_block_bitmap_csum_lo;
    uint16_t  bg_inode_bitmap_csum_lo;
    uint16_t  bg_itable_unused_lo;
    uint16_t  bg_checksum;
    uint32_t  bg_block_bitmap_hi;
    uint32_t  bg_inode_bitmap_hi;
    uint32_t  bg_inode_table_hi;
    uint16_t  bg_free_blocks_count_hi;
    uint16_t  bg_free_inodes_count_hi;
    uint16_t  bg_used_dirs_count_hi;
    uint16_t  bg_itable_unused_hi;
    uint32_t  bg_exclude_bitmap_hi;
    uint16_t  bg_block_bitmap_csum_hi;
    uint16_t  bg_inode_bitmap_csum_hi;
    uint32_t  bg_reserved;
} block_descriptor_t;

#define BLOCK_DESCRIPTOR_SIZE sizeof(block_descriptor_t)

typedef struct inode {
	uint16_t i_mode;
	uint16_t i_uid;
	uint32_t i_size_lo;
	uint32_t i_atime;
	uint32_t i_ctime;
	uint32_t i_mtime;
	uint32_t i_dtime;
	uint16_t i_gid;
	uint16_t i_links_count;
	uint32_t i_blocks_lo;
	uint32_t i_flags;
	union {
		struct {
			uint32_t l_i_version;
		} linux1;
		struct {
			uint32_t h_i_translator;
		} hurd1;
		struct {
			uint32_t m_i_reserved1;
		} masix1;
	} osd1;
	uint32_t i_block[15];
	uint32_t i_generation;
	uint32_t i_file_acl_lo;
	uint32_t i_size_high;
	uint32_t i_obso_faddr;
	union {
		struct {
			uint16_t l_i_blocks_high;
			uint16_t l_i_file_acl_high;
			uint16_t l_i_uid_high;
			uint16_t l_i_gid_high;
			uint16_t l_i_checksum_lo;
			uint16_t l_i_reserved;
		} linux2;
		struct {
			uint16_t h_i_reserved1;
			uint16_t h_i_mode_high;
			uint16_t h_i_uid_high;
			uint16_t h_i_gid_high;
			uint32_t h_i_author;
		} hurd2;
		struct {
			uint16_t h_i_reserved1;
			uint16_t m_i_file_acl_high;
			uint32_t m_i_reserved2[2];
		} masix2;
	} osd2;
	uint16_t i_extra_isize;
	uint16_t i_checksum_hi;
	uint32_t i_ctime_extra;
	uint32_t i_mtime_extra;
	uint32_t i_atime_extra;
	uint32_t i_crtime;
	uint32_t i_crtime_extra;
	uint32_t i_version_hi;
	uint32_t i_projid;
	uint8_t i_cos_reserved[96];
} inode_t;

typedef struct ext4_extent_header {
	uint16_t eh_magic;
	uint16_t eh_entries;
	uint16_t eh_max;
	uint16_t eh_depth;
	uint32_t eh_generation;
} extent_head_t;

typedef struct ext4_extent_idx {
	uint32_t ei_block;
	uint32_t ei_leaf_lo;
	uint16_t ei_leaf_hi;
	uint16_t ei_unused;
} extent_internal_t;

typedef struct ext4_extent {
	uint32_t ee_block;
	uint16_t ee_len;
	uint16_t ee_start_hi;
	uint32_t ee_start_lo;
} extent_leaf_t;

typedef struct ext4_extent_tail {
	uint32_t eb_checksum;
} extent_tail_t;

typedef struct ext4_dir_entry {
	uint32_t inode;
	uint16_t rec_len;
	uint16_t name_len;
	char name[EXT4_NAME_LEN];
} dirent_t;

typedef struct ext4_dir_entry_2 {
	uint32_t inode;
	uint16_t rec_len;
	uint8_t name_len;
	uint8_t file_type;
	char name[EXT4_NAME_LEN];
} dirent2_t;

typedef struct ext4_extended_dir_entry_2 {
	uint32_t hash;
	uint32_t minor_hash;
} extended_dirent2_t;

typedef struct ext4_dir_entry_tail {
	uint32_t det_reserved_zero;
	uint16_t det_rec_len;
	uint8_t det_reserved_zero2;
	uint8_t det_reserved_ft;
	uint32_t det_checksum;
} dirent2_tail_t;

#define INODE_STRUCTURE_SIZE sizeof(inode_t)

typedef struct ext_tree {
    extent_head_t ext_head;
    union {
        extent_internal_t ext_node_first;
        extent_leaf_t ext_leaf_first;
    } node1;
    union {
        extent_internal_t ext_node_second;
        extent_leaf_t ext_leaf_second;
    } node2;
    union {
        extent_internal_t ext_node_third;
        extent_leaf_t ext_leaf_third;
    } node3;
    union {
        extent_internal_t ext_node_fourth;
        extent_leaf_t ext_leaf_fourth;
    } node4;
} tree_t;

typedef struct ext_node {
    union {
		extent_head_t ext_head;
        extent_internal_t ext_internal;
        extent_leaf_t ext_leaf;
    } node;
} ext4_node_t;

#define TREE_SIZE sizeof(extent_leaf_t)