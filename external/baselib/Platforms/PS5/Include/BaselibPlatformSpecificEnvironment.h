#pragma once

#ifndef EXPORTED_SYMBOL
    #define EXPORTED_SYMBOL __declspec(dllexport)
#endif
#ifndef IMPORTED_SYMBOL
    #define IMPORTED_SYMBOL __declspec(dllimport)
#endif

#ifndef PLATFORM_FUTEX_NATIVE_SUPPORT
    #define PLATFORM_FUTEX_NATIVE_SUPPORT 0
#endif

// PS5 specified alignment to be at least 32 byte.
// It also requires this of any user defined malloc implementation (user_malloc).
#ifndef PLATFORM_MEMORY_MALLOC_MIN_ALIGNMENT
    #define PLATFORM_MEMORY_MALLOC_MIN_ALIGNMENT 32
#endif
