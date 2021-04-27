#pragma once

#if IL2CPP_TARGET_PS4

#include "PS4/AtomicImpl-c-api.h"
#include <sce_atomic.h>

namespace il2cpp
{
namespace os
{
    inline void Atomic::FullMemoryBarrier()
    {
        sceAtomicMemoryBarrier();
    }
}
}

#endif
