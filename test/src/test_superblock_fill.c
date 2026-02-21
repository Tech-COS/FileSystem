//
// Created by user on 1/19/26.
//

#include "blocks_functions.h"
#include "blocks_structures.h"

void test_superblock_fill()
{
    char disk_address[4096];
    size_t filesystem_size = 4096;
    size_t offset = 0;
    uint64_t timestamp = 0;

    
}

void run_superblock_tests(void)
{
    if (TEST_PROTECT()) {
        RUN_TEST(test_cos_syswrite);
    }
}
