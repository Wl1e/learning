#include <learning/net/poller.h>
#include <learning/net/poller/epollPoller.h>
#include <learning/net/poller/pollPoller.h>

using namespace learning::net;

poller* poller::defaultPoller(eventLoop* loop)
{
    if(getenv("USE_POLL")) {
        return new pollPoller(loop);
    }
    else {
        return new epollPoller(loop);
    }
}