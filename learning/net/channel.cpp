#include <learning/net/channel.h>
#include <learning/net/eventLoop.h>
#include <learning/base/logging.h>

#include <assert.h>
#include <poll.h>

using namespace learning;
using namespace learning::net;

namespace learning::net {

const uint32_t readEvent = POLLIN | POLLPRI;
const uint32_t writeEvent = POLLOUT;
const uint32_t noneEvent = 0;

} // namespace learning::net

channel::channel(eventLoop* loop, int fd)
  : fd_(fd),
    loop_(loop),
    events_(0),
    revents_(0),
    eventHanding_(false),
    isTied_(false),
    logHup_(false),
    index_(-1)
{
    assert(fd > 0);
}

void channel::handle(timeStamp t)
{
    if(!isTied_ || tie_.lock()) {
        _handleWithoutGuard(t);
    }
}

channel::~channel()
{
    assert(!addInLoop_);
    assert(!eventHanding_);
}

void channel::_update()
{
    addInLoop_ = true;
    loop_->updateChannel(this);
}

void channel::_handleWithoutGuard(timeStamp t)
{
    eventHanding_ = true;
    LOG_TRACE << reventsToString();
    if((revents_ & POLLHUP) && !(revents_ & POLLIN)) {
        if(logHup_) {
            LOG_WARN << "fd = " << fd_ <<  " POLLHUP";
        }
        if(closeCallback_) {
            closeCallback_();
        }
    }
    if(revents_ & POLLNVAL)
    {
        LOG_WARN << "fd = " << fd_ <<  " POLLNVAL";
    }
    if(revents_ & (POLLNVAL | POLLERR)) {
        if(errorCallback_) {
            errorCallback_();
        }
    }
    if(revents_ & (POLLIN | POLLRDHUP | POLLPRI)) {
        if(messageCallback_) {
            messageCallback_(t);
        }
    }
    if(revents_ & POLLOUT) {
        if(writeCallback_) {
            writeCallback_();
        }
    }
    eventHanding_ = false;
}

void channel::remove()
{
    loop_->assertInLoopThread();
    loop_->removeChannel(this);
    addInLoop_ = false;
}

std::string channel::eventsToString() const
{ return _eventToString(fd_, events_); }
std::string channel::reventsToString() const
{ return _eventToString(fd_, revents_); }

std::string channel::_eventToString(int fd, int ev)
{
    std::ostringstream oss;
    oss << fd << ": ";
    if (ev & POLLIN)
        oss << "IN ";
    if (ev & POLLPRI)
        oss << "PRI ";
    if (ev & POLLOUT)
        oss << "OUT ";
    if (ev & POLLHUP)
        oss << "HUP ";
    if (ev & POLLRDHUP)
        oss << "RDHUP ";
    if (ev & POLLERR)
        oss << "ERR ";
    if (ev & POLLNVAL)
        oss << "NVAL ";

    return oss.str();
}