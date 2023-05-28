#ifndef LEARNING_NET_TIMER_H
#define LEARNING_NET_TIMER_H

#include <learning/base/noncopyable.h>
#include <learning/base/timeStamp.h>
#include <learning/net/callback.h>

#include <functional>

namespace learning{
namespace net{

class timer: noncopyable
{
public:
    timer(timeStamp time,
          double interval,
          const net::timerCallback& cb)
      : time_(time),
        repeat_(interval != 0),
        interval_(interval),
        cb_(cb)
    {}

    void run() const { cb_(); }
    bool isRepeat() const
    { return repeat_; }
    void reset(timeStamp nowTime)
    {
        if(isRepeat()) {
            time_ = nowTime + interval_;
        }
        else {
            time_ = timeStamp::invalid();
        }
    }
    timeStamp getTime() const
    { return time_; }

private:
    timeStamp time_;
    bool repeat_;
    double interval_; /* second */
    net::timerCallback cb_;
};

} // namespace net
} // namespace learning

#endif // LEARNING_NET_TIMER_H