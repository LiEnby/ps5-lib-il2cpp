#include "MultiplexIO.h"
#include "os\Posix\PosixHelpers.h"

#include <libnet\epoll.h>
#include <libnet\errno.h>

#define MAX_EVENTS (16)

namespace il2cpp
{
namespace Sockets
{
    static struct { short unixval; uint32_t sceval; }
    vals[] = { { POLLIN, SCE_NET_EPOLLIN }, { POLLOUT, SCE_NET_EPOLLOUT }, { POLLERR, SCE_NET_EPOLLERR }, { POLLHUP, SCE_NET_EPOLLHUP }, { POLLNVAL, SCE_NET_EPOLLERR } };

    static uint32_t RemapUnixToSCEEvents(short unixEvents)
    {
        uint32_t sceEvents = 0;
        for (int bit = 0; bit < sizeof(vals) / sizeof(vals[0]); bit++)
        {
            if (unixEvents & vals[bit].unixval)
                sceEvents |= vals[bit].sceval;
        }
        return sceEvents;
    }

    static short RemapSCEToUnixEvents(uint32_t sceEvents)
    {
        uint32_t unixEvents = 0;
        for (int bit = 0; bit < sizeof(vals) / sizeof(vals[0]); bit++)
        {
            if (sceEvents & vals[bit].sceval)
                unixEvents |= vals[bit].unixval;
        }
        return unixEvents;
    }

    MultiplexIO::MultiplexIO()
    {
        m_epollId = sceNetEpollCreate("AsyncSocketPoll", 0);
    }

    MultiplexIO::~MultiplexIO()
    {
        sceNetEpollDestroy(m_epollId);
    }

    void MultiplexIO::InterruptPoll()
    {
        sceNetEpollAbort(m_epollId, SCE_NET_EPOLL_ABORT_FLAG_PRESERVATION);
    }

    // we return revents field in requests if appropriate
    os::WaitStatus MultiplexIO::Poll(std::vector<il2cpp::os::PollRequest> &requests, int32_t timeout, int32_t *result, int32_t *error)
    {
        const int32_t nfds = (int32_t)requests.size();

        if (nfds < 1)
        {
            //              printf("polling for an unexpected number of requests:%d\n", nfds);
            return kWaitStatusFailure;      //
        }

        SceNetEpollEvent epoll_events[MAX_EVENTS];
        int intfds_idx, nevents, ret;

        int events_idx = 0;

        // Since control does not like the same FD added twice we have to merge
        //  sets applying to the same FD
        memset(&epoll_events, 0, sizeof(SceNetEpollEvent) * MAX_EVENTS);

        for (int fds_idx = 0; fds_idx < nfds; fds_idx++)
        {
            if (events_idx >= MAX_EVENTS)
                break;

            int64_t reqFd = requests[fds_idx].fd;
            if (reqFd != -1)
            {
                // go through events to see if it's already in the list ... if not add to end
                int j = 0;
                for (j = 0; j < events_idx; j++)
                {
                    if (epoll_events[j].ident == reqFd)     // if we found a match we add to it
                        break;
                }

                // if not found in the list, it's a new one so increment number in array
                if (j == events_idx)
                {
                    events_idx++;
                    if (events_idx >= MAX_EVENTS)
                    {
                        printf("exceeded supported number of polling events\n");
                        return kWaitStatusFailure;
                    }
                }

                epoll_events[j].ident = requests[fds_idx].fd;
                epoll_events[j].events |= RemapUnixToSCEEvents(os::posix::PollFlagsToPollEvents(requests[fds_idx].events));     // events is a bitmask so we have to accumulate bits using OR
            }
        }

        // 2nd pass go through merged events adding them ... there should be no duplicates
        for (int k = 0; k < events_idx; k++)
        {
            //              printf("add epollid 0x%x socket:0x%x event:0x%x\n", m_epollId, epoll_events[k].ident, epoll_events[k].events);
            ret = sceNetEpollControl(m_epollId, SCE_NET_EPOLL_CTL_ADD, epoll_events[k].ident, &epoll_events[k]);
            if (ret < 0)
            {
                printf("error during sceNetEpollControl 0x%x\n", ret);
            }
        }


        SceNetEpollEvent epoll_result_events[MAX_EVENTS];

        // calculate and handle timeout (milliseconds -> microseconds)
        timeout = (timeout >= 0) ? timeout * 10000 : -1;

        nevents = 0;
        // waits for events specified in earlier SCE_NET_EPOLL_CTL_ADD controls, 2nd parameter is output only
        ret = sceNetEpollWait(m_epollId, epoll_result_events, MAX_EVENTS, timeout);

        if (ret < 0)
        {
            if (ret == SCE_NET_ERROR_EINTR)
            {
                //                  printf("sceNetEpollWait eid:0x%x returned SCE_NET_ERROR_EINTR (wait has been canceled)\n", m_epollId);
            }
            else
            {
                //                  printf("sceNetEpollWait returned without cancel\n");
            }
        }
        else
        {
            nevents = ret;
        }


        for (int k = 0; k < events_idx; k++)
        {
            ret = sceNetEpollControl(m_epollId, SCE_NET_EPOLL_CTL_DEL, epoll_events[k].ident, 0);
            if (ret < 0)
                printf("error during sceNetEpollControl del 0x%x\n", ret);
        }

        // go through the requests matching with results
        for (int fds_idx = 0; fds_idx < nfds; fds_idx++)
        {
            SceNetId netId = requests[fds_idx].fd;

            for (int k = 0; k < nevents; k++)
            {
                if (epoll_result_events[k].ident == netId)
                {
                    int resultevents = os::posix::PollEventsToPollFlags(RemapSCEToUnixEvents(epoll_result_events[k].events));
                    int mask = requests[fds_idx].events & resultevents;     // create a mask of events that are requested and resulted
                    requests[fds_idx].revents = (os::PollFlags)(((int)requests[fds_idx].revents) | mask);
                }
            }
        }
        return kWaitStatusSuccess;
    }

    int MultiplexIO::poll(pollfd *ufds, unsigned int nfds, int timeout)
    {
        SceNetEpollEvent epoll_events[MAX_EVENTS];
        int intfds_idx, nevents, ret;

        int events_idx = 0;

        // Since control does not like the same FD added twice we have to merge
        //  sets applying to the same FD
        memset(&epoll_events, 0, sizeof(SceNetEpollEvent) * MAX_EVENTS);

        for (int fds_idx = 0; fds_idx < nfds; fds_idx++)
        {
            if (events_idx >= MAX_EVENTS)
                break;

            epoll_events[events_idx].ident = ufds[fds_idx].fd;
            epoll_events[events_idx].events = RemapUnixToSCEEvents(ufds[fds_idx].events);

            if (epoll_events[events_idx].events)
            {
                ret = sceNetEpollControl(m_epollId, SCE_NET_EPOLL_CTL_ADD, epoll_events[events_idx].ident, &epoll_events[events_idx]);
                if (ret < 0)
                {
                    printf("error during sceNetEpollControl 0x%x\n", ret);
                }
                events_idx++;
            }
        }

        // calculate and handle timeout (milliseconds -> microseconds)
        timeout = (timeout >= 0) ? timeout * 10000 : -1;

        nevents = 0;
        ret = sceNetEpollWait(m_epollId, epoll_events, MAX_EVENTS, timeout);


        if ((ret == SCE_NET_ERROR_EINTR) || (ret < 0))
        {
            //              printf("sceNetEpollWait aborted:0x%x", ret);
        }
        else
        {
            nevents = ret;
        }

        // revoke the add commands
        for (int fds_idx = 0; fds_idx < nfds; fds_idx++)
        {
            uint32_t sceEvents = RemapUnixToSCEEvents(ufds[fds_idx].events);
            if (sceEvents)
            {
                int revokeRes = sceNetEpollControl(m_epollId, SCE_NET_EPOLL_CTL_DEL, ufds[fds_idx].fd, 0);
                if (revokeRes < 0)
                {
                    printf("error during sceNetEpollControl del 0x%x\n", revokeRes);
                    ret = revokeRes;
                }
            }


            for (events_idx = 0; events_idx < nevents; events_idx++)
            {
                if (epoll_events[events_idx].ident == ufds[fds_idx].fd)
                {
                    ufds[fds_idx].revents = RemapSCEToUnixEvents(epoll_events[events_idx].events);
                    break;
                }
            }
        }


        return ret;
    }
}
}
