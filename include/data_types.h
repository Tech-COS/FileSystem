#pragma once

#define UINT8_SIZE  1
#define UINT16_SIZE 2
#define UINT32_SIZE 4
#define UINT64_SIZE 8
#define COS_EXIT_FAILURE 84
#define COS_EXIT_SUCCESS 0
#define SECTOR_SIZE 256
#define WRITE_MODE 0
#define READ_MODE 1
#define ROOT_INODE_INDEX 2

//file type 
#define FILE_TYPE_UNKNOWN 0
#define FILE_TYPE_REGULAR_FILE 1
#define FILE_TYPE_DIRECTORY 2
#define FILE_TYPE_CHARACTER_DEVICE_FILE 3
#define FILE_TYPE_CHARACTER_BLOCK_FILE 4
#define FILE_TYPE_FIFO 5
#define FILE_TYPE_SOCKET 6
#define FILE_TYPE_SYMBOLIC_LINK 7

#define CONTENT_ACCESSED 1
#define CONTENT_NOT_ACCESSED 0


typedef unsigned char       uint8_t;
typedef unsigned short      uint16_t;
typedef unsigned int        uint32_t;
typedef unsigned long       uint64_t;
typedef unsigned long       size_t;
typedef signed long       ssize_t;

typedef signed char         int8_t;
typedef signed short        int16_t;
typedef signed int          int32_t;
typedef signed long         int64_t;

typedef unsigned char       *uptr8_t;
typedef unsigned short      *uptr16_t;
typedef unsigned int        *uptr32_t;
typedef unsigned long       *uptr64_t;

typedef signed char         *ptr8_t;
typedef signed short        *ptr16_t;
typedef signed int          *ptr32_t;
typedef signed long         *ptr64_t;
