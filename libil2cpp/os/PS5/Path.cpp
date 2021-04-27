#include "il2cpp-config.h"

#include <scebase.h>
#include <kernel.h>
#include <app_content.h>
#include <libsysmodule.h>

#include "os/Path.h"

#include <string>

extern bool g_TemporaryDataIsMounted;
extern SceAppContentMountPoint g_TemporaryDataMountPoint;


namespace il2cpp
{
namespace os
{
    std::string Path::GetExecutablePath()
    {
        return std::string();
    }

    std::string Path::GetTempPath()
    {
        if (g_TemporaryDataIsMounted)
            return g_TemporaryDataMountPoint.data;
        return std::string("<GetTempPath is not implemented>");
    }

    bool Path::IsAbsolute(const std::string& path)
    {
        return path[0] == '/';
    }
}
}
