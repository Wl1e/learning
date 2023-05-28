#include <learning/net/poller/epollPoller.h>
#include <learning/net/channel.h>
#include <learning/base/logging.h>
#include <learning/base/timeStamp.h>

using namespace learning;
using namespace learning::net;

const int defaultMaxPollSize = 16;

enum STATE{
    NEW = -1,
    ADDED = 1
};

epollPoller::epollPoller(eventLoop* loop)
  : poller(loop, defaultMaxPollSize),
    epollFd_(epoll_create1(EPOLL_CLOEXEC))
    {
        if(epollFd_ < 0) {
            LOG_SYSFATAL << "create epollFd error";
        }
        events_.resize(maxPollSize());
    }
epollPoller::~epollPoller()
{
    close(epollFd_);
}
timeStamp epollPoller::poll(int timeoutMS, channelList* activeChannel)
{
    LOG_TRACE << "fd total count = " << channels_.size();
    int n = epoll_wait(epollFd_, &*events_.begin(), static_cast<int>(events_.size()), timeoutMS);
    timeStamp now = timeStamp::now();
    int savedError = errno;
    if(n > 0) {
        if(static_cast<size_t>(n) >= maxPollSize()) {
            LOG_ERROR << "pollSize is small";
            // events_.resize(events_.size());
        }
        LOG_TRACE << n << " events happened";
        _fullActiveChannel(n, activeChannel);
    }
    else if(n == 0) {
        LOG_INFO << "nothing happened";
    }
    else {
        if(savedError != EINTR) {
            errno = savedError;
            LOG_SYSFATAL << "epoll_wait error";
        }
    }
    return now;
}

void epollPoller::updateChannel(channel* pChannel)
{
    assert(pChannel);
    LOG_TRACE << "fd = " << pChannel->fd() << " events = " << pChannel->events() << " index = " << pChannel->index();
    if(pChannel->index() == NEW) {
        int fd = pChannel->fd();
        assert(channels_.count(fd) == 0);
        pChannel->setIndex(ADDED);
        _update(EPOLL_CTL_ADD, pChannel);
        channels_[fd] = pChannel;
    }
    else {
        int fd = pChannel->fd();
        assert(channels_.count(fd) != 0 && channels_[fd] == pChannel);
        if(pChannel->isNoneEvent()) {
            pChannel->setIndex(NEW);
            _update(EPOLL_CTL_DEL, pChannel);
            channels_.erase(fd);
        }
        else {
            _update(EPOLL_CTL_MOD, pChannel);
        }
    }
}

std::string ctlToString(int ctl)
{
    switch(ctl) {
        case EPOLL_CTL_ADD: {
            return "ADD";
        }
        case EPOLL_CTL_MOD: {
            return "MOD";
        }
        case EPOLL_CTL_DEL: {
            return "DEL";
        }
        default: {
            return "unknown";
        }
    }
}

void epollPoller::_update(int ctl, channel* pChannel)
{
    int fd = pChannel->fd();
    epoll_event newEvent;
    memset(&newEvent, 0, sizeof newEvent);
    newEvent.data.ptr = pChannel;
    newEvent.events = pChannel->events();
    LOG_TRACE << "epoll_ctl op = " << ctlToString(ctl)
              << " fd = " << fd << " event = {" << pChannel->eventsToString() << "}" << " index = " << pChannel->index();
    if(epoll_ctl(epollFd_, ctl, fd, &newEvent) != 0) {
        LOG_SYSFATAL << "epoll_ctl error, ctl = " << ctlToString(ctl) << ", fd = " << fd;
    }
}

void epollPoller::removeChannel(channel* pChannel)
{
    assert(pChannel);
    int fd = pChannel->fd();
    assert(channels_.count(fd) && channels_[fd] == pChannel);
    if(epoll_ctl(epollFd_, EPOLL_CTL_DEL, fd, nullptr) != 0)
    {
        LOG_SYSFATAL << "error" << ", fd = " << fd;
    }
    channels_.erase(fd);
    pChannel->setIndex(NEW);
}

void epollPoller::_fullActiveChannel(size_t size, channelList* activeChannel)
{
    assert(activeChannel);
    assert(size <= events_.size());
    for(size_t i{0}; i != size; ++i) {
        channel* pChannel = static_cast<channel*>(events_[i].data.ptr);
        pChannel->setRevents(events_[i].events);
        activeChannel->push_back(pChannel);
    }
}


