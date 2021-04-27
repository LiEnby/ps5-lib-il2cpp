#include "os/Assert.h"

#if IL2CPP_DEBUG

#if IL2CPP_TARGET_PS5

#include <cstdio>
#include <cstdlib>

void il2cpp_assert(const char* assertion, const char* file, unsigned int line)
{
    printf("Assertion failed: %s, file %s, line %u\n", assertion, file, line);
    quick_exit(1);
}

#endif // IL2CPP_TARGET_PS5

#endif // IL2CPP_DEBUG
