#include "ext4_functions.h"
#include "blocks_functions.h"
#include "data_types.h"
#include <time.h>

int main(int argc, char **argv)
{
    char *file;
    size_t size = 0;
    size_t offset = 0;
    uint64_t timestamp = time(NULL);

    if (argc != 2)
        return COS_EXIT_FAILURE;
    if (!(file = read_file(argv[1], &size)))
        return COS_EXIT_FAILURE;
    write_superblock_to_disk(file, size, &offset, timestamp);
    write_block_descriptor(file, size, &offset);
    write_inode_to_disk(file, size, &offset, timestamp);
    write_to_disk(argv[1], file, size);
    return COS_EXIT_SUCCESS;
}
