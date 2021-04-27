#include "FilePlatformConfig.h"
#include <kernel.h>
#include <stdio.h>


int lstat(const char *file_name, struct stat *buf)
{
    return stat(file_name, buf);
}

int utime(__const char *__file, __const struct utimbuf *__file_times)
{
    SceKernelTimeval times[2];
    times[0].tv_sec = __file_times->actime;
    times[1].tv_sec = __file_times->modtime;
    int res = sceKernelUtimes(__file, &times[0]);
    if (res == 0)
        return 0;

    return -1;
}

void * mmap(void *addr, size_t length, int prot, int flags,
                  int fd, off_t offset)
{
    return NULL;
}
