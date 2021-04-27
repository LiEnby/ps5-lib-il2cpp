#pragma once

#include <sce_atomic.h>
#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif

static inline int32_t UnityPalAdd(volatile int32_t* location1, int32_t value)
{
    return sceAtomicAdd32AcqRel(location1, value) + value;
}

static inline int64_t UnityPalAdd64(volatile int64_t* location1, int64_t value)
{
    return sceAtomicAdd64AcqRel(location1, value) + value;
}

static inline int32_t UnityPalIncrement(volatile int32_t* value)
{
    return sceAtomicIncrement32AcqRel(value) + 1;
}

static inline int64_t UnityPalIncrement64(volatile int64_t* value)
{
    return sceAtomicIncrement64AcqRel(value) + 1;
}

static inline int32_t UnityPalDecrement(volatile int32_t* value)
{
    return sceAtomicDecrement32AcqRel(value) - 1;
}

static inline int64_t UnityPalDecrement64(volatile int64_t* value)
{
    return sceAtomicDecrement64AcqRel(value) - 1;
}

static inline int32_t UnityPalCompareExchange(volatile int32_t* dest, int32_t exchange, int32_t comparand)
{
    return sceAtomicCompareAndSwap32AcqRel(dest, comparand, exchange);
}

static inline int64_t UnityPalCompareExchange64(volatile int64_t* dest, int64_t exchange, int64_t comparand)
{
    return sceAtomicCompareAndSwap64AcqRel(dest, comparand, exchange);
}

static inline void* UnityPalCompareExchangePointer(void* volatile* dest, void* exchange, void* comparand)
{
    return (void*)sceAtomicCompareAndSwap64AcqRel((volatile int64_t*)dest, (int64_t)comparand, (int64_t)exchange);
}

static inline int64_t UnityPalExchange64(volatile int64_t* dest, int64_t exchange)
{
    return sceAtomicExchange64AcqRel(dest, exchange);
}

static inline int32_t UnityPalExchange(volatile int32_t* dest, int32_t exchange)
{
    return sceAtomicExchange32AcqRel(dest, exchange);
}

static inline void* UnityPalExchangePointer(void* volatile* dest, void* exchange)
{
    return (void*)sceAtomicExchange64AcqRel((volatile int64_t*)dest, (int64_t)exchange);
}

static inline int64_t UnityPalRead64(volatile int64_t* addr)
{
    return sceAtomicLoad64AcqRel(addr);
}

static inline void UnityPalMemoryBarrier()
{
    sceAtomicMemoryBarrier();
}

#if defined(__cplusplus)
}
#endif
