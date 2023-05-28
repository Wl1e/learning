#ifndef LEARNING_NET_EPOLLPOLLER_H
#define LEARNING_NET_EPOLLPOLLER_H

#include <learning/base/noncopyable.h>
#include <learning/net/poller.h>

#include <vector>
#include <sys/epoll.h>

namespace learning {
namespace net {

class eventLoop;
class channel;

class epollPoller: public poller,
                   private noncopyable
{
public:
    epollPoller(eventLoop* loop);
    ~epollPoller();
    timeStamp poll(int timeoutMS, channelList* activeChannel);
    void updateChannel(channel* pChannel);
    void removeChannel(channel* pChannel);

protected:
    void _update(int ctl, channel* pChannel);
    void _fullActiveChannel(size_t size, channelList* activeChannel);

private:
    int epollFd_;
    using eventList = std::vector<epoll_event>;
    eventList events_;
};

} // namespace net
} // namespace learning

#endif // LEARNING_NET_EPOLLPOLLER_H