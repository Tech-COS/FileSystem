#include <stdint.h>


#define STACK_CHK_GUARD 0xa5f3cc8d

uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

__attribute((noreturn)) void __stack_chk_fail(void)
{
    while(1);
    //_exit();
}
