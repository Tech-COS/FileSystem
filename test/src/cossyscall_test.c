#include "cos_syscall.h"
#include "unity.h"

#define UNUSED(x) ((void)x)

void setUp(void) {}
void tearDown(void) {}

void test_cos_syswrite(void)
{
    uint16_t read_data[512] = {0};
    uint16_t write_data[512] = {0};

    for (int i = 0; i < 512; ++i) {
        write_data[i] = 0x0030;
    }

    write_byte(write_data, 1, 2);
    read_byte(read_data, 1, 2);

    for (int i = 0; i < 512; ++i) {
        TEST_ASSERT_EQUAL_INT(0x0030, read_data[i]);
    }
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();
    if (TEST_PROTECT()) {
        RUN_TEST(test_cos_syswrite);
    }
    UNUSED(argc);
    UNUSED(argv);
    return UNITY_END();
}
