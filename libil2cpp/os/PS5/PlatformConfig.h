#define IL2CPP_USE_POSIX_SOCKET_PLATFORM_CONFIG (1)
#define IL2CPP_USE_POSIX_FILE_PLATFORM_CONFIG (1)
#define IL2CPP_USE_POSIX_COND_TIMEDWAIT_REL (1)
#define IL2CPP_USE_SOCKET_MULTIPLEX_IO (1)
#define IL2CPP_USE_SOCKET_SETBLOCKING (1)

#define IL2CPP_ENABLE_NATIVE_STACKTRACES (1)

#define IL2CPP_ENABLE_PLATFORM_THREAD_AFFINTY (1) 

#define IL2CPP_ENABLE_PLATFORM_THREAD_RENAME (1)
#define IL2CPP_ENABLE_PLATFORM_THREAD_STACKSIZE (1)

#define IL2CPP_DEFAULT_STACK_SIZE (256 * 1024)
#define IL2CPP_CUSTOM_PLATFORM (1)

#define IL2CPP_TARGET_PS5 1
#define _UNICODE 1
#define UNICODE 1


 #include <_pthread.h>
 struct cpu_set_t { long __bits[__bitset_words((64))]; };
 int pthread_setaffinity_np(struct pthread_attr** pAttr, int sizeofset, struct cpu_set_t *pCpu_set);
