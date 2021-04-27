#if defined(RUNTIME_MONO)

/* These are functions used by Mono that are missing on PS4. */

#include <assert.h>

int isascii(int ch)
{
    return ch >= 0 && ch < 128;
}

int pipe(int pipefd[2])
{
    assert(0 && "This function is not yet implemented.");
    return -1;
}

int access(const char *path, int mode)
{
    /* Maybe we could use sceKernelStat to implement this. */

    assert(0 && "This function is not yet implemented.");
    return 0;
}

#endif // RUNTIME_MONO
