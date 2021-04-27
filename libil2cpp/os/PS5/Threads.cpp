#include <kernel.h>
#include <cerrno>

int pthread_attr_setaffinity_np(pthread_attr_t *attr, size_t cpusetsize, const cpu_set_t *cpuset)
{
    const int gOrbisThreadinheritsched = SCE_PTHREAD_EXPLICIT_SCHED;
    const int gOrbisThreadpolicy = SCE_KERNEL_SCHED_RR;
    SceKernelCpumask affinityMask = 0x3c;       // core 5,4,3,2

    if ((cpuset != NULL) && (cpusetsize == sizeof(cpu_set_t)))
    {
        affinityMask = (SceKernelCpumask)cpuset->__bits[0];
    }

    int result;
    result = scePthreadAttrSetinheritsched(attr, gOrbisThreadinheritsched);
    if (result != 0)
        return result;

    result = scePthreadAttrSetschedpolicy(attr, gOrbisThreadpolicy);
    if (result != 0)
        return result;

    result = scePthreadAttrSetaffinity(attr, affinityMask);
    return result;
}

int pthread_setaffinity_np(struct pthread_attr** pAttr, int sizeofset, struct cpu_set_t *pCpu_set)
{
    return pthread_attr_setaffinity_np(pAttr, sizeofset, pCpu_set);
}

int pthread_setname_np(pthread_t handle, const char *name)
{
    return scePthreadRename(handle, name);
}

/*
    Specification to usec can be made in microsecond units, however, the precision of actual processing is in millisecond units.
*/
int     pthread_cond_timedwait_relative_np(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *spec)
{
    SceKernelUseconds usec = spec->tv_sec * (1000 * 1000);
    usec += (spec->tv_nsec / 1000);
    int result = scePthreadCondTimedwait(cond, mutex, usec);
    if (result == SCE_KERNEL_ERROR_ETIMEDOUT)
        return ETIMEDOUT;

    return result;
}
