#ifndef LEARNING_NET_CHANNEL_H
#define LEARNING_NET_CHANNEL_H

#include <learning/base/noncopyable.h>
#include <learning/base/timeStamp.h>

#include <memory>
#include <functional>
#include <sstream>
#include <poll.h>

namespace learning {
namespace net {

class eventLoop;

extern const uint32_t readEvent;
extern const uint32_t writeEvent;
extern const uint32_t noneEvent;

class channel: noncopyable
{
public:
    channel(eventLoop* loop, int fd);
    ~channel();

    void enableReading()
    { events_ |= readEvent; _update(); }
    void enableWriting()
    { events_ |= writeEvent; _update(); }
    void disableReading()
    { events_ &= ~readEvent; _update(); }
    void disableWriting()
    { events_ &= ~writeEvent; _update(); }
    void remove();
    void disableAll()
    { events_ = noneEvent; }
    bool isReading() const
    { return events_ & readEvent; }
    bool isWriting() const
    { return events_ & writeEvent; }

    int fd() const
    { return fd_; }
    eventLoop* loop() const
    { return loop_; }
    uint32_t events() const
    { return events_; }
    uint32_t revents() const
    { return revents_; }
    void setRevents(uint32_t events)
    { revents_ = events; }
    std::string eventsToString() const;
    std::string reventsToString() const;
    bool isNoneEvent() const
    { return events() == noneEvent; }
    int index() const
    { return index_; }

    void setIndex(int index)
    { index_ = index; }
    using messageCallback = std::function<void(timeStamp)>;
    using eventCallback = std::function<void()>;
    void setMessageCallback(const messageCallback& cb)
    { messageCallback_ = cb; }
    void setWriteCallback(const eventCallback& cb)
    { writeCallback_ = cb; }
    void setErrorCallback(const eventCallback& cb)
    { errorCallback_ = cb; }
    void setCloseCallback(const eventCallback& cb)
    { closeCallback_ = cb; }

    void handle(timeStamp t);

    void tie(const std::weak_ptr<void>& tie)
    { tie_ = tie; isTied_ = true; }

private:
    void _update();
    void _handleWithoutGuard(timeStamp t);
    static std::string _eventToString(int fd, int ev);

private:
    int fd_;
    eventLoop* loop_;

    uint32_t events_;
    uint32_t revents_;
    bool eventHanding_;
    bool addInLoop_;

    messageCallback messageCallback_;
    eventCallback writeCallback_;
    eventCallback errorCallback_;
    eventCallback closeCallback_;

    std::weak_ptr<void> tie_;
    bool isTied_;

    bool logHup_;
    int index_;
};

} // namespace net
} //namespace learning

#endif // LEARNING_NET_CHANNEL_H