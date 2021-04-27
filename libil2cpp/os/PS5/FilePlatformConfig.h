#include "sys\poll.h"

// enable POSIX specific file headers
#include <unistd.h>
#include <sys\errno.h>

#if !defined(_UNISTD_H_)
#error unistd.h not included correctly
#endif

extern int errno;

struct utimbuf
{
    __time_t actime;        /* Access time.  */
    __time_t modtime;       /* Modification time.  */
};

int utime(__const char *__file, __const struct utimbuf *__file_times);

#define isatty(x) (0)

//int lstat(const char *file_name, struct stat *buf);
