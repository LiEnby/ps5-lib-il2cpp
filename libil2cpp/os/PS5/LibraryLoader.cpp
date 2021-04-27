#include "il2cpp-config.h"

#include <kernel.h>

#include <string>
#include <cassert>

#include "il2cpp-metadata.h"
#include "os/LibraryLoader.h"
#include "vm/Exception.h"
#include "vm/PlatformInvoke.h"

extern size_t g_args;
extern const void *g_argp;

namespace il2cpp
{
namespace os
{
    struct LibraryNamePrefixAndSuffix
    {
        LibraryNamePrefixAndSuffix(const char* prefix_, const char* suffix_)
        {
            prefix = std::string(prefix_);
            suffix = std::string(suffix_);
        }

        std::string prefix;
        std::string suffix;
    };

    static LibraryNamePrefixAndSuffix LibraryNamePrefixAndSuffixVariations[1] =
    {
        LibraryNamePrefixAndSuffix("", ".prx")
    };

    static Baselib_DynamicLibrary_Handle LoadLibraryWithName(const char* name, std::string& detailedError)
    {
        int res;
        char buffer[292]; //SCE_FIOS_OVERLAY_POINT_MAX

        // if string starts with / then take it as an absolute string, otherwise add the default plugin locations
        if (name[0] == '/')
        {
            snprintf(buffer, sizeof(buffer), "%s", name);
        }
        else
        {
            extern std::string GetDataFolder();
            snprintf(buffer, sizeof(buffer), "%s/Plugins/%s", GetDataFolder().c_str(), name);
        }

        // add a .prx if there isn't one already there
        int length = strlen(buffer);
        if (strcasecmp(buffer + (length - 4), ".prx") != 0)
        {
            strcat(buffer, ".prx");
        }

        return LibraryLoader::TryOpeningLibrary(buffer, detailedError);
    }

    static Baselib_DynamicLibrary_Handle CheckLibraryVariations(const char* name, std::string& detailedError)
    {
        int numberOfVariations = sizeof(LibraryNamePrefixAndSuffixVariations) / sizeof(LibraryNamePrefixAndSuffixVariations[0]);
        for (int i = 0; i < numberOfVariations; ++i)
        {
            std::string libraryName = LibraryNamePrefixAndSuffixVariations[i].prefix + name + LibraryNamePrefixAndSuffixVariations[i].suffix;
            auto handle = LoadLibraryWithName(libraryName.c_str(), detailedError);
            if (handle != Baselib_DynamicLibrary_Handle_Invalid)
                return handle;
        }

        return Baselib_DynamicLibrary_Handle_Invalid;
    }

#define unamesize 20
    struct utsname
    {
        char sysname[unamesize];
        char nodename[unamesize];
        char release[unamesize];
        char version[unamesize];
        char machine[unamesize];
    };

    int uname(utsname *name)
    {
        if (name)
        {
            strncpy(name->sysname , "ps5", unamesize);
            strncpy(name->nodename, "ps5", unamesize);
            strncpy(name->release, "ps5", unamesize);
            strncpy(name->version, "ps5", unamesize);
            strncpy(name->machine, "ps5", unamesize);
            return 0;
        }
        return -1;
    }

    int getdomainname(char *name, size_t len)
    {
        if (name)
        {
            strncpy(name, "ps5", len);
            return 0;
        }
        return -1;
    }

    const HardcodedPInvokeDependencyFunction kLibcFunctions[] =
    {
        HARDCODED_DEPENDENCY_FUNCTION(uname),
        HARDCODED_DEPENDENCY_FUNCTION(getdomainname),
    };

    const HardcodedPInvokeDependencyLibrary kHardcodedPInvokeDependencies[] =
    {
        HARDCODED_DEPENDENCY_LIBRARY("libc", kLibcFunctions),
    };

    const HardcodedPInvokeDependencyLibrary* LibraryLoader::HardcodedPInvokeDependencies = kHardcodedPInvokeDependencies;
    const size_t LibraryLoader::HardcodedPInvokeDependenciesCount = sizeof(kHardcodedPInvokeDependencies) / sizeof(HardcodedPInvokeDependencyLibrary);

    Baselib_DynamicLibrary_Handle LibraryLoader::ProbeForLibrary(const Il2CppNativeChar* libraryName, const size_t libraryNameLength, std::string& detailedError)
    {
        auto handle = LoadLibraryWithName(libraryName, detailedError);

        if (handle == Baselib_DynamicLibrary_Handle_Invalid)
            handle = CheckLibraryVariations(libraryName, detailedError);

        if (handle == Baselib_DynamicLibrary_Handle_Invalid)
        {
            size_t lengthWithoutDotDll = libraryNameLength - 4;
            if (strncmp(libraryName + lengthWithoutDotDll, ".dll", 4) == 0)
            {
                char* nativeDynamicLibraryWithoutExtension = static_cast<char*>(alloca(lengthWithoutDotDll + 1));
                memcpy(nativeDynamicLibraryWithoutExtension, libraryName, lengthWithoutDotDll);
                nativeDynamicLibraryWithoutExtension[lengthWithoutDotDll] = 0;

                handle = CheckLibraryVariations(nativeDynamicLibraryWithoutExtension, detailedError);
            }
        }

        return handle;
    }

    Baselib_DynamicLibrary_Handle LibraryLoader::OpenProgramHandle(Baselib_ErrorState& errorState, bool& /*needsClosing*/)
    {
        // this feature is not supported on PS5, but we use baselib to generate a graceful error code.
        return Baselib_DynamicLibrary_OpenProgramHandle(&errorState);
    }
}
}
