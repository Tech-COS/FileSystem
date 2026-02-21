/*
** EPITECH PROJECT, 2025
** COS
** File description:
** stat
*/

#ifndef STAT_H_
#define STAT_H_

typedef struct {
    uint64_t st_dev;           // Device ID
    uint64_t st_ino;           // Inode number
    uint16_t st_mode;          // File type and mode
    uint16_t st_nlink;         // Number of hard links
    uint32_t st_uid;           // User ID of owner
    uint32_t st_gid;           // Group ID of owner
    uint64_t st_rdev;          // Device ID (if special file)
    uint64_t st_size;          // Total size, in bytes
    uint64_t st_blksize;       // Block size for filesystem I/O
    uint64_t st_blocks;        // Number of 512B blocks allocated
    uint64_t st_atime;         // Time of last access
    uint64_t st_mtime;         // Time of last modification
    uint64_t st_ctime;         // Time of last status change
    uint64_t st_crtime;        // Time of creation (birth time)
} stat_t;

int cos_stat(const char *path, stat_t *statbuf);

#endif /* !STAT_H_ */
