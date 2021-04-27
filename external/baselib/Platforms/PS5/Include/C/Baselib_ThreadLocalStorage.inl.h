#pragma once

#include <kernel.h>

#ifdef __cplusplus
BASELIB_C_INTERFACE
{
#endif

BASELIB_FORCEINLINE_API void Baselib_TLS_Set(Baselib_TLS_Handle handle, uintptr_t value)
{
    int rc = scePthreadSetspecific((ScePthreadKey)handle, (void*)value);
    BaselibAssert(rc == 0);
}

BASELIB_FORCEINLINE_API uintptr_t Baselib_TLS_Get(Baselib_TLS_Handle handle)
{
    return (uintptr_t)scePthreadGetspecific((ScePthreadKey)handle);
}

#ifdef __cplusplus
} // BASELIB_C_INTERFACE
#endif
