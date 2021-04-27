//#include "Sockets.h"
#include "SocketImplPlatformConfig.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <net.h>
#include <scetypes.h>
#include <libnet/in.h>
#include <libnet/socket.h>
#include "FilePlatformConfig.h"

#include "os/Socket.h"
#include "os/WaitStatus.h"
#include "os/Posix/PosixHelpers.h"
#include <vector>

#include <libnetctl.h>
#include <arpa/inet.h>


#include "MultiplexIO.h"

int gethostname(char *name,  int namelen)
{
    SceNetCtlInfo info;
    int ret = sceNetCtlGetInfo(SCE_NET_CTL_INFO_IP_ADDRESS, &info);
    if (ret < 0)
    {
        return -1;
    }

    strncpy(name, info.ip_address, namelen);
    return 0;
}

struct hostent* gethostbyname(const char *hostname)
{
    static __thread hostent result;
    static __thread char name[SCE_NET_RESOLVER_HOSTNAME_LEN_MAX + 1];
    static __thread char *aliases[2];
    aliases[0] = NULL;
    aliases[1] = NULL;
    strncpy(name, hostname, SCE_NET_RESOLVER_HOSTNAME_LEN_MAX);


    int poolId = sceNetPoolCreate("resolver", 8 * 1024, 0);
    SceNetId rid = -1;
    int ret;
    rid = sceNetResolverCreate("resolver", poolId, 0);
    if (rid < 0)
    {
        return (NULL);
    }


    static __thread SceNetInAddr addr;
    static __thread SceNetInAddr *addr_list[2];
    addr_list[0] = &addr;
    addr_list[1] = NULL;

    int timeout_us = 0;
    int retry = 0;
    int flags = 0;
    ret = sceNetResolverStartNtoa(rid, name, &addr, timeout_us, retry, flags);
    if (ret < 0)
    {
        sceNetResolverDestroy(rid);
        int ret1 = sceNetPoolDestroy(poolId);
        return (NULL);
    }
    ret = sceNetResolverDestroy(rid);
    int ret1 = sceNetPoolDestroy(poolId);
    if ((ret < 0) || (ret1 < 0))
    {
        return (NULL);
    }


    result.h_length = 4;
    result.h_addrtype = AF_INET;
    result.h_name = name;
    result.h_aliases = (char**)&aliases;
    result.h_addr_list = (char**)&addr_list;

    return &result;
}

unsigned long inet_addr(const char *cp)
{
    SceNetInAddr_t dst;
    sceNetInetPton(SCE_NET_AF_INET, cp, &dst);
    return dst;
}

#define SF_BUFFER_SIZE (4096)
#define SOCKET_ERROR (-1)


ssize_t sendfile(int out_fd, int in_fd, off_t *offset, size_t count)
{
    /* Default implementation */
    void *buffer;
    int n;

    buffer = malloc(SF_BUFFER_SIZE);
    do
    {
        do
        {
            n = read(in_fd, buffer, SF_BUFFER_SIZE);
        }
        while (n == -1 && errno == EINTR);
        if (n == -1)
            break;
        if (n == 0)
        {
            free(buffer);
            return 0; /* We're done reading */
        }
        do
        {
            n = send(out_fd, buffer, n, 0); /* short sends? enclose this in a loop? */
        }
        while (n == -1 && sce_net_errno == SCE_NET_EAGAIN);
    }
    while (n != -1);

    if (n == -1)
    {
        free(buffer);
        return SOCKET_ERROR;
    }
    free(buffer);
    return 0;
}

struct hostent * gethostbyaddr(const void *addrInp, int lenInp, int type)
{
    if (type != AF_INET)
        return NULL;

    static hostent result;
    static char name[SCE_NET_RESOLVER_HOSTNAME_LEN_MAX + 1];
    static char *aliases[2];
    aliases[0] = NULL;
    aliases[1] = NULL;

    const int addrSize = 32;
    static char *addrs[2];
    static char addr[addrSize];
    addrs[0] = addr;
    addrs[1] = NULL;

    int len = lenInp < addrSize ? lenInp : addrSize;
    memcpy(addr, addrInp, len);
    in_addr * inaddr = (in_addr*)addr;


    int poolId = sceNetPoolCreate("resolver", 8 * 1024, 0);
    SceNetId rid = -1;
    int ret;
    rid = sceNetResolverCreate("resolver", poolId, 0);
    if (rid < 0)
    {
        return (NULL);
    }

    int timeout_us = 0;
    int retry = 0;
    int flags = 0;
    ret = sceNetResolverStartAton(rid, (const SceNetInAddr*)inaddr, name, SCE_NET_RESOLVER_HOSTNAME_LEN_MAX + 1, timeout_us, retry, flags);
    if (ret < 0)
    {
        unsigned char *byteaddress = (unsigned char*)inaddr;
        snprintf(name, SCE_NET_RESOLVER_HOSTNAME_LEN_MAX + 1, "%d.%d.%d.%d", byteaddress[0], byteaddress[1], byteaddress[2], byteaddress[3]);
        sceNetResolverDestroy(rid);
        return (NULL);
    }
    ret = sceNetResolverDestroy(rid);
    int ret1 = sceNetPoolDestroy(poolId);
    if ((ret < 0) || (ret1 < 0))
    {
        return (NULL);
    }


    result.h_length = 4;
    result.h_addrtype = AF_INET;
    result.h_name = name;
    result.h_aliases = (char**)&aliases;
    result.h_addr_list = (char**)&addrs;

    return &result;
}

struct in_addr *get_local_ips(int32_t family, int32_t *nips)
{
    static in_addr localip;
    SceNetCtlInfo info;
    sceNetCtlGetInfo(SCE_NET_CTL_INFO_IP_ADDRESS, &info);


    inet_pton(AF_INET, info.ip_address, &localip);

    if (nips)
        *nips = 1;

    return &localip;
}

int ioctl(int fd, unsigned long request, ...)
{
    int result = -1;
    va_list args;
    va_start(args, request);

    switch (request)
    {
        case FIONREAD:
        {
            int32_t * amount = va_arg(args, int32_t *);
            *amount = 0;
            SceNetSockInfo sockinfo;
            int res = sceNetGetSockInfo(fd, &sockinfo, 1, 0);
            if (res > 0)
            {
                *amount = sockinfo.recv_queue_length;
                result = 0;
            }
        }
        break;
        case SIOCGIFCONF:
        {
            ifconf * ifc = va_arg(args, ifconf *);
            if (ifc->ifc_len >= sizeof(ifconf))
            {
                SceNetCtlInfo info;
                in_addr localip;
                int32_t offset = 4;
                sceNetCtlGetInfo(SCE_NET_CTL_INFO_IP_ADDRESS, &info);
                inet_pton(AF_INET, info.ip_address, &localip);
                ifc->ifc_len = sizeof(ifconf);
                strcpy(ifc->ifc_req[0].ifr_name, "net0");
                memcpy(ifc->ifc_req[0].ifr_addr.sa_data + offset, &localip, sizeof(localip));
                result = 0;
            }
        }
        break;

        case SIOCGIFFLAGS:
        {
        }
        break;


        default:
            break;
    }
    va_end(args);
    return result;
}

int dup2(int oldfd, int newfd)
{
    return 0;
}

/*
    Only suitable for polling network sockets

    Inputs:
    timeout: the number of milliseconds that poll() should block waiting for socket to become ready
*/
int poll(pollfd *ufds, unsigned int nfds, int timeout)
{
    int result = -1;
    if (nfds <= 0)
    {
        printf("polling %d sockets ... aborting\n", nfds);
    }
    else
    {
        il2cpp::Sockets::MultiplexIO poller;
        result = poller.poll(ufds, nfds, timeout);
    }


    return result;
}

int setBlocking(int _fd, bool blocking)
{
    int notblocking = blocking ? 0 : 1;
    int ret = sceNetSetsockopt(_fd, SCE_NET_SOL_SOCKET, SCE_NET_SO_NBIO, (void*)&notblocking, sizeof(notblocking));
    if (ret != 0)
    {
        printf("sceNetSetsockopt failed with 0x%x\n", ret);
    }

    int result;
    SceNetSocklen_t optlen = sizeof(result);
    ret = sceNetGetsockopt(_fd, SCE_NET_SOL_SOCKET, SCE_NET_SO_NBIO, (void*)&result, &optlen);
    if (ret != 0)
    {
        printf("sceNetGetsockopt failed with 0x%x\n", ret);
    }
    result = (result == 0) ? 0 : 1;
    if (result != notblocking)
    {
        return 1;   // 1 = error
    }
    return 0;
}
