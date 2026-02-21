#pragma once

#include <stdbool.h>

#include "blocks_structures.h"
#include "data_types.h"

struct ext4_dir_entry_2_cos;

extern void *memset(void *ptr, int character, size_t size);
extern void cos_memcpy(void *dest_buffer, void *source_buffer, size_t data_size);
extern ssize_t cos_strcmp(const char *str1, const char *str2);
extern void cos_printf(const char *, ...);
extern void extract_directory_path(const char *path, char *dir_path);
extern void extract_filename(const char *path, char *filename);
extern uint32_t find_directory_inode(const char *path, bool display_error_messages, struct ext4_dir_entry_2_cos *extracted_dirent);

char *read_file(const char *file, size_t *size);
void write_to_disk(const char *file, char *buffer, size_t buf_size);
void read_inode_tree(tree_t *tree, uint8_t **content, size_t *len, size_t *offset, uint8_t mode);
size_t little_endian_reversal(size_t value, size_t data_size);
void write_wrapper(uint64_t inode_nb, uint8_t *content, size_t len, size_t offset);
void __stack_chk_fail(void);
void read_wrapper(uint64_t inode_nb, uint8_t *content, size_t len, size_t offset, bool is_content_read);
bool read(const char *path, uint8_t *content, size_t len, size_t offset);
uint64_t get_first_null_inode(void);

