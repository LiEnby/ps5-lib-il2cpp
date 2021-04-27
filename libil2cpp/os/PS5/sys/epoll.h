#define SCE_NET_COMPAT_EPOLL64
#include <net.h>

#define epoll_event SceNetEpollEvent
#define EPOLLIN SCE_NET_EPOLLIN
#define EPOLLOUT SCE_NET_EPOLLOUT
#define EPOLLERR SCE_NET_EPOLLERR
#define EPOLLHUP SCE_NET_EPOLLHUP
#define EPOLL_CTL_ADD SCE_NET_EPOLL_CTL_ADD
#define EPOLL_CTL_DEL SCE_NET_EPOLL_CTL_DEL
#define EPOLL_CTL_MOD SCE_NET_EPOLL_CTL_MOD
#define epoll_create(n) sceNetEpollCreate("threadpool_epoll",0)
#define EPOOL_CLOEXEC
#define EPOLL_CLOEXEC 0
#define epoll_create1(n) sceNetEpollCreate("threadpool_epoll",0)
#define epoll_ctl sceNetEpollControl
#define epoll_wait sceNetEpollWait
#define EPOLL_NEVENTS (32)
//      #define EPOLL_DEBUG
#define epoll_ctl_error(a) ((a)<0)
#define CLOSE sceNetSocketClose
