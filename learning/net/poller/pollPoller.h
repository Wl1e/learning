#ifndef LEARNING_NET_POLLPOLLER_H
#define LEARNING_NET_POLLPOLLER_H

#include <learning/base/noncopyable.h>

#include <learning/net/poller.h>

#include <poll.h>

namespace learning {
namespace net {

class pollPoller: public poller,
                  private noncopyable
{
public:
    pollPoller(eventLoop* loop);
    ~pollPoller();
    timeStamp poll(int timeoutMS, channelList* activeChannel);
    void updateChannel(channel*);
    void removeChannel(channel*);

private:
    void _fullActiveChannel(size_t size, channelList* activeChannel);

private:
    using pollfdList = std::vector<pollfd>;
    pollfdList pollfds_;
};

} // namespace net
} //namespace learning

#endif // LEARNING_NET_POLLPOLLER_H