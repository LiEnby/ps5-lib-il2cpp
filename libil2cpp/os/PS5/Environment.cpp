#include "il2cpp-config.h"


#include "il2cpp-class-internals.h"
#include "os/Environment.h"
#include "utils/Expected.h"
#include "il2cpp-api.h"

#include <cassert>
#include <stdlib.h>

#include <map>


#include "os/PS5/SocketImplPlatformConfig.h"
#include "os/PS5/Environment.h"


typedef std::map<std::string, std::string> EnvironmentVariablesMap;
EnvironmentVariablesMap *s_env = NULL;
static char s_datafolder[292] = { 0 }; //SCE_FIOS_OVERLAY_POINT_MAX
extern "C" __declspec(dllexport) void SetDataFolder(std::string folder)
{
    strncpy(s_datafolder, folder.c_str(), sizeof(s_datafolder));
    s_datafolder[strlen(folder.c_str())] = 0;
}

extern "C" __declspec(dllexport) int  setenv(const char* envname, const char *envval, int overwrite)
{
    if (s_env == NULL)
    {
        s_env = new EnvironmentVariablesMap();
    }
    if ((envval == NULL) || (envval[0] == 0))
    {
        s_env->erase(envname);
    }
    else
    {
        if (overwrite == 0)
        {
            EnvironmentVariablesMap::iterator it = s_env->find(envname);
            if (it != s_env->end())
            {
                return 0;
            }                                           // if we find it, don't overwrite
        }
        s_env->insert(std::pair<std::string, std::string>(envname, envval));
    }
    return 0;
}

extern "C" __declspec(dllexport) const char *getenv(const char* envname)
{
    if (s_env == NULL)
    {
        return NULL;
    }
    EnvironmentVariablesMap::iterator it = s_env->find(envname);
    if (it == s_env->end())
    {
        return NULL;
    }
    return it->second.c_str();
}

extern "C" __declspec(dllexport) int unsetenv(const char* envname)
{
    if (s_env != NULL)
        s_env->erase(envname);
    return 0;
}

namespace il2cpp
{
namespace os
{
    std::string Environment::GetMachineName()
    {
        char buf[256];

        if (gethostname(buf, sizeof(buf)) != 0)
            return "";

        return buf;
    }

    int32_t Environment::GetProcessorCount()
    {
        int count = 6;

        return count;
    }

    std::string Environment::GetOsVersionString()
    {
        return "0.0.0.0";
    }

    std::string Environment::GetOsUserName()
    {
        const std::string username(GetEnvironmentVariable("USER"));
        return username.empty() ? "Unknown" : username;
    }

    std::string Environment::GetEnvironmentVariable(const std::string& name)
    {
        const char* variable = getenv(name.c_str());
        return variable ? std::string(variable) : std::string();
    }

    void Environment::SetEnvironmentVariable(const std::string& name, const std::string& value)
    {
        if (value.empty())
        {
            unsetenv(name.c_str());
        }
        else
        {
            setenv(name.c_str(), value.c_str(), 1); // 1 means overwrite
        }
    }

    std::vector<std::string> Environment::GetEnvironmentVariableNames()
    {
        std::vector<std::string> result;
        if (s_env != NULL)
        {
            for (EnvironmentVariablesMap::iterator it = s_env->begin(); it != s_env->end(); ++it)
            {
                result.push_back(std::string(it->first));
            }
        }
        return result;
    }

    std::string Environment::GetHomeDirectory()
    {
        static std::string homeDirectory;

        if (!homeDirectory.empty())
            return homeDirectory;

        homeDirectory = GetEnvironmentVariable("HOME");

        return homeDirectory.empty() ? "/" : homeDirectory;
    }

    std::vector<std::string> Environment::GetLogicalDrives()
    {
        std::vector<std::string> result;

        result.push_back("/");
        return result;
    }

    void Environment::Exit(int result)
    {
        exit(result);
    }

    utils::Expected<std::string> Environment::GetWindowsFolderPath(int folder)
    {
        // This should only be called on Windows.
        return std::string();
    }

    //  We use this one for anything that isn't going via FIOS (we manually remap by altering path)
    std::string GetDataFolder()
    {
        return s_datafolder;
    }

    NORETURN void Environment::Abort()
    {
        quick_exit(1);
    }
}
}
