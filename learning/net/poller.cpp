#include <learning/net/poller.h>
#include <learning/net/channel.h>

using namespace learning;
using namespace learning::net;

poller::poller(eventLoop* loop, size_t maxPollSize)
  : loop_(loop),
    maxPollSize_(maxPollSize)
{}

poller::~poller() {}

bool poller::hasChannel(channel* pChannel)
{
    // loop_->assertInLoopThread();
    int fd = pChannel->fd();
    auto it = channels_.find(fd);
    if(it != channels_.end()) {
        return fd == it->second->fd();
    }
    return false;
}