#pragma once


#include <net.h>
#include <netinet\in.h>
#include <sys/socket.h>     // posix sockets


#undef SUPPORT_UNIXSOCKETS
#define SUPPORT_UNIXSOCKETS (0)


// redefine SOCK_CLOSE to use sce version
#undef SOCK_CLOSE
#define SOCK_CLOSE sceNetSocketClose

// PS4 does not allow handling of socket exceptions like SIGPIPE, so they must be disabled during send() or process will abort
#undef IL2CPP_USE_SEND_NOSIGNAL
#define IL2CPP_USE_SEND_NOSIGNAL (1)

struct hostent
{
    char    *h_name;
    char    **h_aliases;
    short   h_addrtype;
    short   h_length;
    char    **h_addr_list;
};


int gethostname(char *name, int len);
struct hostent* gethostbyname(const char *hostname);
struct hostent *gethostbyaddr(const void *addr, int len, int type);
struct in_addr *get_local_ips(int32_t family, int32_t *nips);


// missing posix functions defined in Sockets.cpp
unsigned long inet_addr(const char *cp);
ssize_t sendfile(int out_fd, int in_fd, off_t *offset, size_t count);
//int ioctl(int fd, unsigned long request, ...);
int dup2(int oldfd, int newfd);

// non posix functions
int setBlocking(int socket, bool blocking);


struct ip_mreqn     // see ip_mreq on ps4
{struct  in_addr imr_multiaddr;
 struct  in_addr imr_address; };


#define IFNAMSIZ (8)
struct ifreq
{
    char ifr_name[IFNAMSIZ]; /* Interface name */
    union
    {
        struct sockaddr ifr_addr;
        struct sockaddr ifr_dstaddr;
        struct sockaddr ifr_broadaddr;
        struct sockaddr ifr_netmask;
        struct sockaddr ifr_hwaddr;
        short          ifr_flags;
        int        ifr_ifindex;
        int        ifr_metric;
        int        ifr_mtu;
        //      struct ifmap    ifr_map;
        char           ifr_slave[IFNAMSIZ];
        char           ifr_newname[IFNAMSIZ];
        char *         ifr_data;
    };
};

struct ifconf
{
    int           ifc_len; /* size of buffer */
    union
    {
        char *        ifc_buf; /* buffer address */
        struct ifreq * ifc_req; /* array of structures */
    };
};

#define IFF_UP  (1 << 0)
#define IFF_LOOPBACK (1 << 3)


// set up dummy values so that FIONREAD will compile
#define IOCPARM_MASK    0x7ff            /* parameters must be < 2k bytes */
#define IOCGROUP(x)     (((x) >> 8) & 0xff)
#define IOC_VOID        0x20000000      /* no parameters */
#define IOC_OUT     0x40000000  /* copy out parameters */
#define IOC_IN      0x80000000  /* copy in parameters */
#define IOC_INOUT   (IOC_IN|IOC_OUT)
#define _IOWR(g, n, t)    _IOC(IOC_INOUT,   (g), (n), sizeof(t))
#define _IOC(inout, group, num, len) \
        (inout | ((len & IOCPARM_MASK) << 16) | ((group) << 8) | (num))
#define _IOR(g, n, t)     _IOC(IOC_OUT,   (g), (n), sizeof(t))
#define FIONREAD        _IOR('f', 127, int)     /* get # bytes to read */
#define SIOCGIFCONF _IOWR('i',36, struct ifconf)    /* get ifnet list */
#define SIOCGIFFLAGS    _IOWR('i',17, struct ifreq) /* get ifnet flags */
