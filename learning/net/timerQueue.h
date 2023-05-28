#ifndef LEARNING_NET_TIMERQUEUE_H
#define LEARNING_NET_TIMERQUEUE_H

#include <learning/base/noncopyable.h>
#include <learning/net/timer.h>

#include <vector>
#include <assert.h>
#include <map>
#include <set>
#include <memory>
#include <atomic>

namespace learning {
namespace net{

class eventLoop;
class channel;

class timerQueue: noncopyable
{
public:
    explicit timerQueue(net::eventLoop* loop);
    ~timerQueue();

    timer* add(timeStamp time,
             double interval,
             const net::timerCallback& cb);
    // int nextTime();
    void del(timer* timer);

private:

    void _handle();
    void _addTimerInLoop(timer*);
    void _getActiveTimer(timeStamp nowTime);
    void _reset(timeStamp nowTime);

private:
    net::eventLoop* loop_;

    int fd_;
    std::unique_ptr<net::channel> timerfdChannel_;
    std::atomic_bool callngExpiredTimers_;

    using entry = std::pair<timeStamp, timer*>;
    std::set<entry> queue_;
    std::set<timer*> timers_;
    std::vector<timer*> activeTimers_;
};

} // namespace net
} //namespace learning

#endif // LEARNING_NET_TIMERQUEUE_H