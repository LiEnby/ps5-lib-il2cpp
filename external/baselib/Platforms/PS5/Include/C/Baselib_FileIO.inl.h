#pragma once

typedef enum Baselib_FileIO_NativeHandleType
{
    // file descriptor returned by sceKernelOpen
    Baselib_FileIO_NativeHandleType_SceKernelFD = 1,
} Baselib_FileIO_NativeHandleType;
