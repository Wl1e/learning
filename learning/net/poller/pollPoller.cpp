#include <learning/net/poller/pollPoller.h>
#include <learning/net/eventLoop.h>
#include <learning/net/socketOps.h>
#include <learning/net/channel.h>
#include <learning/base/logging.h>
#include <learning/base/timeStamp.h>

#include <poll.h>

using namespace learning;
using namespace learning::net;

const size_t defaultMaxPollSize = 4096;

pollPoller::pollPoller(eventLoop* loop)
  : poller(loop, defaultMaxPollSize)
{}

pollPoller::~pollPoller() = default;

timeStamp pollPoller::poll(int timeoutMS, channelList* activeChannel)
{
    int ret = ::poll(&pollfds_[0], pollfds_.size(), timeoutMS);
    timeStamp now(timeStamp::now());
    int savedErrno = errno;
    if(ret > 0) {
        _fullActiveChannel(ret, activeChannel);
    }
    else if(ret == 0) {
        LOG_INFO << "no poller";
    }
    else {
        errno = savedErrno;
        LOG_SYSFATAL << "poll: error";
    }
    return now;
}
void pollPoller::updateChannel(channel* pChannel)
{
    assert(pChannel);
    if(pChannel->index() == -1) {
        int fd = pChannel->fd();
        assert(channels_.count(fd) == 0);
        if(pollfds_.size() >= maxPollSize()) {
            LOG_ERROR << "pollfds full";
        }
        pollfd newPollfd;
        newPollfd.events = static_cast<short>(pChannel->events());
        newPollfd.fd = fd;
        newPollfd.revents = 0;
        pollfds_.push_back(newPollfd);
        pChannel->setIndex(static_cast<int>(pollfds_.size() - 1));
        channels_.emplace(fd, pChannel);
    }
    else {
        int index = pChannel->index();
        assert(index >= 0);
        assert(index < pollfds_.size());
        int fd = pChannel->fd(); (void)fd;
        assert(channels_.count(fd) != 0 && channels_[fd] == pChannel);
        pollfd& thisPollfd = pollfds_[index];
        thisPollfd.events = static_cast<short>(pChannel->events());
    }
}
void pollPoller::removeChannel(channel* pChannel)
{
    assert(pChannel);
    int index = pChannel->index();
    if(index == -1) {
        LOG_ERROR << "this channel was not added to pollfds";
    }
    assert(index >= 0);
    assert(index < pollfds_.size());
    int fd = pChannel->fd();
    assert(channels_.count(fd) != 0);
    assert(channels_[fd] == pChannel);
    /*  有没有可能出现race condition,
        这里交换之后, channel的index暂时没有改变, 但是channel调用update, 修改了别的channel的pollfd
        加个atomic_bool swaping_ ? */
    std::swap(pollfds_[index], pollfds_.back());
    pollfd& thisPollfd = pollfds_[index];
    channels_[thisPollfd.fd]->setIndex(index);
    pChannel->setIndex(-1);
    pollfds_.pop_back();
    channels_.erase(fd);
}

void pollPoller::_fullActiveChannel(size_t size, channelList* activeChannel)
{
    assert(activeChannel);
    assert(size <= pollfds_.size());
    for(size_t i{0}; i != pollfds_.size() && size != 0; ++i) {
        if(pollfds_[i].revents != 0) {
            int fd = pollfds_[i].fd;
            activeChannel->push_back(channels_[fd]);
            --size;
        }
    }
    if(size != 0) {
        LOG_ERROR << "nums is error?";
    }
}