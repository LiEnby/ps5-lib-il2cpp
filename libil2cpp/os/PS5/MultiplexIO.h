#include "sys/poll.h"
#include "os/socket.h"
#include <libnet/nettypes.h>

namespace il2cpp
{
namespace Sockets
{
    class MultiplexIO
    {
    public:
        MultiplexIO();
        ~MultiplexIO();

        os::WaitStatus Poll(std::vector<il2cpp::os::PollRequest> &requests, int32_t timeout, int32_t *result, int32_t *error);
        int poll(pollfd *ufds, unsigned int nfds, int timeout);
        void InterruptPoll();

    private:
        SceNetId m_epollId;
    };
}
}
