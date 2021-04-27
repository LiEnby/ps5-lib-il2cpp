#pragma once

extern "C" __declspec(dllexport) int  unsetenv(const char*a);
extern "C" __declspec(dllexport) const char *  getenv(const char*a);
extern "C" __declspec(dllexport) int  setenv(const char* envname, const char *envval, int overwrite);
extern "C" __declspec(dllexport) void SetDataFolder(std::string folder);
