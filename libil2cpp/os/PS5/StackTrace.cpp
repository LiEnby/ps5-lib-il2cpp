#include "il2cpp-config.h"

#include "os/StackTrace.h"

#include <sdk_version.h>
#if SCE_ORBIS_SDK_VERSION > 0x06000000u
#define USE_NEW_BACKTRACE (1)
#endif

#if USE_NEW_BACKTRACE
#include "os/Image.h"
#include <sce_backtrace.h>
#else
#include <libdbg.h>
__declspec(dllimport) extern "C" int32_t arch_DbgBacktraceSelf(SceDbgCallFrame *pCallFrameBuffer, size_t numBytesBuffer, uint32_t *pNumReturn, SceDbgBacktraceMode mode);
#endif


namespace il2cpp
{
namespace os
{
    void StackTrace::WalkStackNative(WalkStackCallback callback, void* context, WalkOrder walkOrder)
    {
#if USE_NEW_BACKTRACE
        int ret;

        uint32_t max = SCE_LIBC_BACKTRACE_FULL;
        uint32_t depth;
        size_t   bufsize;
        ret = sceLibcBacktraceGetBufferSize(max, &depth, &bufsize);
        if (ret != 0)
        {
            return;
        }

        SceLibcCallFrame *frame = (SceLibcCallFrame *)calloc(1, bufsize);
        if (frame == NULL)
        {
            return;
        }

        uint32_t num;
        ret = sceLibcBacktraceSelf(depth, frame, bufsize, &num);
        if (ret != 0)
        {
            free(frame);
            return;
        }

        intptr_t imageBase = (intptr_t)il2cpp::os::Image::GetImageBase();

        if (walkOrder != WalkOrder::kFirstCalledToLastCalled)
        {
            SceLibcCallFrame *frame2 = frame;
            for (int i = 0; i < num; i++)
            {
                if (strcmp(frame2->moduleName, "Il2CppUserAssemblies.prx") == 0)
                {
                    intptr_t fulladdress = imageBase + frame2->offsetInModule;
                    if (!callback(reinterpret_cast<Il2CppMethodPointer>(fulladdress), context))
                        break;
                }

                frame2 = frame2->next;
            }
        }
        else
        {
            for (int i = num - 1; i > 0; i--)
            {
                SceLibcCallFrame *frame2 = frame;
                for (int j = 0; j < i; j++)
                {
                    frame2 = frame2->next;
                }

                if (strcmp(frame2->moduleName, "Il2CppUserAssemblies.prx") == 0)
                {
                    intptr_t fulladdress = imageBase + frame2->offsetInModule;
                    if (!callback(reinterpret_cast<Il2CppMethodPointer>(fulladdress), context))
                        break;
                }
            }
        }

        free(frame);
#else

        const uint32_t kMaxFrames = 128;
        SceDbgCallFrame stack[kMaxFrames];


        uint32_t frames = 0;
        int result = arch_DbgBacktraceSelf(stack, sizeof(stack), &frames, SCE_DBG_BACKTRACE_MODE_DONT_EXCEED);

        if (result < 0)
        {
            return;
        }
        if (walkOrder == WalkOrder::kFirstCalledToLastCalled)
        {
            for (size_t i = frames; i--;)
            {
                if (!callback(reinterpret_cast<Il2CppMethodPointer>(stack[i].pc), context))
                    break;
            }
        }
        else
        {
            for (size_t i = 0; i < frames; i++)
            {
                if (!callback(reinterpret_cast<Il2CppMethodPointer>(stack[i].pc), context))
                    break;
            }
        }
#endif
    }
    
#if IL2CPP_ENABLE_NATIVE_STACKTRACES
    std::string StackTrace::NativeStackTrace()
    {
        return std::string();
    }
#endif

    const void* StackTrace::GetStackPointer()
    {
        // TODO implement to avoid extra WalkStack calls
        return nullptr;
    }
}
}
