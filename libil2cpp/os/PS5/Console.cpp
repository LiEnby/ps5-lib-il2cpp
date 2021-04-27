#include "il2cpp-config.h"

#include "os/Console.h"

namespace il2cpp
{
namespace os
{
namespace Console
{
    int32_t InternalKeyAvailable(int32_t ms_timeout)
    {
        return -1;
    }

    bool SetBreak(bool wantBreak)
    {
        return true;
    }

    bool SetEcho(bool wantEcho)
    {
        return true;
    }

    bool TtySetup(const std::string& keypadXmit, const std::string& teardown, uint8_t* control_characters, int32_t** size)
    {
        return true;
    }
}
}
}
