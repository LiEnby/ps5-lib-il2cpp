#include "il2cpp-config.h"


#include "os/Directory.h"
#include "os/ErrorCodes.h"
#include "os/File.h"
#include "os/Posix/Error.h"
#include "utils/DirectoryUtils.h"
#include "utils/PathUtils.h"
#include "utils/StringUtils.h"
#include <assert.h>
#include <errno.h>
#include <kernel.h>
#include <sceerror.h>

namespace il2cpp
{
namespace os
{
    static  std::string currentDirectory;// no concept of current directory on this platform, so we have to emulate it


    static ErrorCode sceErrnoToErrorCode(int32_t code)
    {
        ErrorCode ret;

        switch (code)
        {
            case SCE_KERNEL_ERROR_EACCES: case SCE_KERNEL_ERROR_EPERM: case SCE_KERNEL_ERROR_EROFS:
                ret = kErrorCodeAccessDenied;
                break;

            case SCE_KERNEL_ERROR_EAGAIN:
                ret = kErrorCodeSharingViolation;
                break;

            case SCE_KERNEL_ERROR_EBUSY:
                ret = kErrorCodeLockViolation;
                break;

            case SCE_KERNEL_ERROR_EEXIST:
                ret = kErrorCodeFileExists;
                break;

            case SCE_KERNEL_ERROR_EINVAL: case SCE_KERNEL_ERROR_ESPIPE:
                ret = kErrorSeek;
                break;

            case SCE_KERNEL_ERROR_EISDIR:
                ret = kErrorCodeCannotMake;
                break;

            case SCE_KERNEL_ERROR_ENFILE: case SCE_KERNEL_ERROR_EMFILE:
                ret = kErrorCodeTooManyOpenFiles;
                break;

            case SCE_KERNEL_ERROR_ENOENT: case SCE_KERNEL_ERROR_ENOTDIR:
                ret = kErrorCodeFileNotFound;
                break;

            case SCE_KERNEL_ERROR_ENOSPC:
                ret = kErrorCodeHandleDiskFull;
                break;

            case SCE_KERNEL_ERROR_ENOTEMPTY:
                ret = kErrorCodeDirNotEmpty;
                break;

            case SCE_KERNEL_ERROR_ENOEXEC:
                ret = kErrorBadFormat;
                break;

            case SCE_KERNEL_ERROR_ENAMETOOLONG:
                ret = kErrorCodeFileNameExcedRange;
                break;

#ifdef SCE_KERNEL_ERROR_EINPROGRESS
            case SCE_KERNEL_ERROR_EINPROGRESS:
                ret = kErrorIoPending;
                break;
#endif

            case SCE_KERNEL_ERROR_ENOSYS:
                ret = kErrorNotSupported;
                break;

            case SCE_KERNEL_ERROR_EBADF:
                ret = kErrorCodeInvalidHandle;
                break;

            case SCE_KERNEL_ERROR_EIO:
                ret = kErrorCodeInvalidHandle;
                break;

            case SCE_KERNEL_ERROR_EINTR:
                ret = kErrorIoPending;
                break;

            case SCE_KERNEL_ERROR_EPIPE:
                ret = kErrorCodeWriteFault;
                break;

            default:
                ret = kErrorCodeGenFailure;
                break;
        }

        return ret;
    }

    std::string Directory::GetCurrent(int *error)
    {
        *error = kErrorCodeSuccess;
        return currentDirectory;
    }

    bool Directory::SetCurrent(const std::string& path, int *error)
    {
        currentDirectory = std::string(path);
        *error = kErrorCodeSuccess;
        return true;
    }

    bool Directory::Create(const std::string& path, int *error)
    {
        const int ret = sceKernelMkdir(path.c_str(), SCE_KERNEL_S_IRWU);

        if (ret != SCE_OK)
        {
            *error = sceErrnoToErrorCode(ret);
            return false;
        }

        *error = kErrorCodeSuccess;
        return true;
    }

    bool Directory::Remove(const std::string& path, int *error)
    {
        const int ret = sceKernelRmdir(path.c_str());

        if (ret != SCE_OK)
        {
            *error = sceErrnoToErrorCode(ret);
            return false;
        }

        *error = kErrorCodeSuccess;
        return true;
    }

    struct DIR
    {
        int fd;
        char *buffer;
        blksize_t st_blksize;
        size_t curpos;
    };

    static void DirectoryGlob(DIR *dir, const std::string& pattern, std::set<std::string>& result)
    {
        if (pattern.empty())
            return;

        std::string matchPattern;
        matchPattern.reserve(pattern.length());

        // Collapse adjacent stars into one
        for (size_t i = 0, length = pattern.length(); i < length; ++i)
        {
            if (i > 0 && pattern[i] == '*' && pattern[i - 1] == '*')
                continue;

            matchPattern.append(1, pattern[i]);
        }

        int bufferused = 0;
        while (1)
        {
            if (dir->curpos >= bufferused)
            {
                int res = sceKernelGetdents(dir->fd, dir->buffer, dir->st_blksize);
                if (res <= 0)
                {
                    break;
                }
                bufferused = res;
                dir->curpos = 0;
            }

            SceKernelDirent *entry = (SceKernelDirent*)(dir->buffer  + dir->curpos);
            dir->curpos += entry->d_reclen;

            // Files that have been deleted or replaced via the patching system have a zero d_fileno.
            if (!entry->d_fileno)
                continue;

            const std::string filename(entry->d_name);

            if (!il2cpp::utils::Match(filename, matchPattern))
                continue;

            result.insert(filename);
        }
    }

    static bool DirectoryGlob(const std::string& directoryPath, const std::string& pattern, std::set<std::string>& result, int* error)
    {
        DIR dirDetails;

        int res = sceKernelOpen(directoryPath.c_str(), SCE_KERNEL_O_RDONLY | SCE_KERNEL_O_DIRECTORY, 0);
        if (res < 0)
        {
            *error = sceErrnoToErrorCode(res);
            return false;
        }
        SceKernelStat sb;
        sceKernelFstat(res, &sb);
        dirDetails.fd = res;
        dirDetails.st_blksize = sb.st_blksize;
        dirDetails.buffer = (char*)malloc(sb.st_blksize);
        dirDetails.curpos = dirDetails.st_blksize; // indicates no valid data

        DirectoryGlob(&dirDetails, pattern, result);


        free(dirDetails.buffer);
        sceKernelClose(dirDetails.fd);

        return true;
    }

    std::set<std::string> Directory::GetFileSystemEntries(const std::string& path, const std::string& pathWithPattern, int32_t attributes, int32_t mask, int* error)
    {
        const std::string directoryPath(il2cpp::utils::PathUtils::DirectoryName(pathWithPattern));
        const std::string pattern(il2cpp::utils::PathUtils::Basename(pathWithPattern));

        std::set<std::string> globResult;

        if (DirectoryGlob(directoryPath, pattern, globResult, error) == false)
            return std::set<std::string>();

        std::set<std::string> result;

        for (std::set<std::string>::const_iterator it = globResult.begin(), end = globResult.end(); it != end; ++it)
        {
            const std::string& filename = *it;

            if (filename == "." || filename == "..")
                continue;

            const std::string path(directoryPath + IL2CPP_DIR_SEPARATOR + filename);

            int attributeError;
            const int32_t pathAttributes = static_cast<int32_t>(File::GetFileAttributes(path, &attributeError));

            if (attributeError != kErrorCodeSuccess)
                continue;

            if ((pathAttributes & mask) == attributes)
                result.insert(path);
        }


        *error = kErrorCodeSuccess;
        return result;
    }

    struct FindHandlePS5
    {
        int fd;
        char* buffer;
        blksize_t st_blksize;
        size_t curpos;
        int bufferused;
    };

    Directory::FindHandle::FindHandle(const utils::StringView<Il2CppNativeChar>& searchPathWithPattern) : osHandle(NULL)
    {
        directoryPath = il2cpp::utils::PathUtils::DirectoryName(searchPathWithPattern);
        pattern = il2cpp::utils::PathUtils::Basename(searchPathWithPattern);
        pattern = il2cpp::utils::CollapseAdjacentStars(pattern);
    }

    Directory::FindHandle::~FindHandle()
    {
        IL2CPP_ASSERT(osHandle == NULL);
    }

    int32_t Directory::FindHandle::CloseOSHandle()
    {
        int32_t result = os::kErrorCodeSuccess;

        if (osHandle)
        {
            FindHandlePS5* handle = (FindHandlePS5*)osHandle;
            sceKernelClose(handle->fd);
            free(handle->buffer);
            free(handle);
            osHandle = NULL;
        }

        return result;
    }

    os::ErrorCode Directory::FindFirstFile(FindHandle* findHandle,
        const utils::StringView<Il2CppNativeChar>& searchPathWithPattern,
        Il2CppNativeString* resultFileName,
        int32_t* resultAttributes)
    {
        int res = sceKernelOpen(findHandle->directoryPath.c_str(), SCE_KERNEL_O_RDONLY | SCE_KERNEL_O_DIRECTORY, 0);
        if (res < 0)
        {
            int errcode = res & 0xffff;  // Note that masking of the hi-word seems to hold true for converting SCE file error codes to
                                         // posix error codes, e.g. SCE_KERNEL_ERROR_ENOENT = 0x80020002 and ENOENT = 2.
            return PathErrnoToErrorCode(findHandle->directoryPath, errcode);
        }

        FindHandlePS5* handle = (FindHandlePS5*)malloc(sizeof(FindHandlePS5));
        SceKernelStat sb;
        sceKernelFstat(res, &sb);
        handle->fd = res;
        handle->st_blksize = sb.st_blksize;
        handle->buffer = (char*)malloc(sb.st_blksize);
        handle->curpos = handle->st_blksize;  // indicates no valid data
        handle->bufferused = 0;

        findHandle->SetOSHandle(handle);

        return FindNextFile(findHandle, resultFileName, resultAttributes);
    }

    os::ErrorCode Directory::FindNextFile(FindHandle* findHandle, Il2CppNativeString* resultFileName, int32_t* resultAttributes)
    {
        FindHandlePS5* handle = (FindHandlePS5*)findHandle->osHandle;

        while (1)
        {
            if (handle->curpos >= handle->bufferused)
            {
                int res = sceKernelGetdents(handle->fd, handle->buffer, handle->st_blksize);
                if (res <= 0)
                {
                    return os::kErrorCodeNoMoreFiles;
                }
                handle->bufferused = res;
                handle->curpos = 0;
            }

            SceKernelDirent* entry = (SceKernelDirent*)(handle->buffer + handle->curpos);
            handle->curpos += entry->d_reclen;

            // Files that have been deleted or replaced via the patching system have a zero d_fileno.
            if (!entry->d_fileno)
                continue;

            const std::string filename(entry->d_name);

            if (il2cpp::utils::Match(filename, findHandle->pattern))
            {
                const Il2CppNativeString path = utils::PathUtils::Combine(findHandle->directoryPath, filename);

                int attributeError;
                const int32_t pathAttributes = static_cast<int32_t>(File::GetFileAttributes(path, &attributeError));

                if (attributeError == kErrorCodeSuccess)
                {
                    *resultFileName = filename;
                    *resultAttributes = pathAttributes;
                    return os::kErrorCodeSuccess;
                }
            }
        }
    }
}
}
