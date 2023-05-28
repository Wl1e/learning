#ifndef LEARNING_NET_POLLER_H
#define LEARNING_NET_POLLER_H

#include <map>
#include <vector>

namespace learning {

class timeStamp;

namespace net {

class eventLoop;
class channel;

class poller
{
public:
    poller(eventLoop* loop, size_t maxPollSize);
    virtual ~poller();

    using channelList = std::vector<channel*>;
    virtual timeStamp poll(int timeoutMS, channelList* activeChannel) = 0;
    bool hasChannel(channel* pChannel);
    virtual void updateChannel(channel* pChannel) = 0;
    virtual void removeChannel(channel* pChannel) = 0;
    void setMaxPollSize(size_t size)
    { maxPollSize_ = size; }
    size_t maxPollSize() const
    { return maxPollSize_; }

    static poller* defaultPoller(eventLoop* loop);

protected:
    virtual void _fullActiveChannel(size_t size, channelList* activeChannel) = 0;

    using channelMap = std::map<int, channel*>;
    channelMap channels_;

private:
    eventLoop* loop_;
    size_t maxPollSize_;
};

} // namespace net
} //namespace learning

#endif // LEARNING_NET_POLLER_H
