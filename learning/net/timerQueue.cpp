#include <learning/net/timerQueue.h>
#include <learning/net/channel.h>
#include <learning/net/eventLoop.h>
#include <learning/base/logging.h>
#include <learning/base/timeStamp.h>

#include <sys/timerfd.h>

using namespace learning;
using namespace learning::net;

namespace learning::detail{

int createTimerfd()
{
    int fd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK);
    if(fd < 0) {
        LOG_SYSFATAL << "in create_timerfd, create error";
    }
    return fd;
}

void readTimerfd(int fd, timeStamp t)
{
    uint64_t num;
    ssize_t n = ::read(fd, &num, sizeof num);
    LOG_TRACE << "n = " << n << "time = " << t.formatToString();
    if(n != sizeof num) {
        LOG_ERROR << "the number maybe error";
    }
}

itimerspec timeStampToTimespec(timeStamp when)
{
    itimerspec tim;
    int64_t ms = when.MSSinceEpoch();
    tim.it_interval.tv_sec = 0;
    tim.it_interval.tv_nsec = 0;
    const int NSPerMs = 1000000;
    tim.it_value.tv_sec = static_cast<time_t>(ms/timeStamp::MSPerSecond);
    tim.it_value.tv_nsec = static_cast<long>((ms%timeStamp::MSPerSecond)*NSPerMs);
    return tim;
}

void setTimerfdTime(int fd, timeStamp t)
{
    itimerspec newTIme;
    newTIme = timeStampToTimespec(t);
    itimerspec oldTime;
    if(!::timerfd_settime(fd, 0, &newTIme, &oldTime)) {
        LOG_SYSERR << "settime error, error = " << errorString(errno);
    }
}

} // namespace learning::detail

timerQueue::timerQueue(net::eventLoop* loop)
  : loop_(loop),
    fd_(detail::createTimerfd()),
    timerfdChannel_(new net::channel(loop_, fd_))
{
    timerfdChannel_->setMessageCallback(std::bind(&timerQueue::_handle, this));
    timerfdChannel_->enableReading();
}

timerQueue::~timerQueue()
{
    timerfdChannel_->disableAll();
    timerfdChannel_->remove();
    close(fd_);
    for(const auto& it : timers_) {
        delete it;
    }
}

timer* timerQueue::add(timeStamp time,
                     double interval,
                     const net::timerCallback& cb)
{
    timer* newTimer = new timer(time, interval, cb);
    loop_->runInLoop(std::bind(&timerQueue::_addTimerInLoop, this, newTimer));
    return newTimer;
}

// int timerQueue::nextTime()
// {
//     int64_t ms = -1;
//     if(!queue_.empty()) {
//         ms = (queue_.begin()->first - timeStamp::now()).count();
//     }
//     return static_cast<int>(ms);
// }

/*  删除后会删除timer指向内容, 禁止使用 */
void timerQueue::del(timer* timer)
{
    assert(timer);
    assert(timers_.count(timer) != 0);
    timeStamp time = timer->getTime();
    auto it = queue_.find({time, timer});
    assert(it != queue_.end());
    assert(it->second == timer);
    /*  或者单纯把cb置为nullptr? */
    queue_.erase(it);
    timers_.erase(timer);
    delete timer;
}

void timerQueue::_addTimerInLoop(timer* pt)
{
    auto it = timers_.find(pt);
    if(it == timers_.cend()) {
        timers_.insert(pt);
        queue_.insert({pt->getTime(), pt});
        detail::setTimerfdTime(fd_, pt->getTime());
    }
}

void timerQueue::_handle()
{
    loop_->assertInLoopThread();
    timeStamp nowTime(timeStamp::now());
    detail::readTimerfd(fd_, nowTime);
    _getActiveTimer(nowTime);
    callngExpiredTimers_ = true;
    for(timer* it : activeTimers_) {
        if(it) {
            it->run();
        }
    }
    callngExpiredTimers_ = false;
    _reset(nowTime);
}

void timerQueue::_getActiveTimer(timeStamp nowTime)
{
    activeTimers_.clear();
    while(!queue_.empty()) {
        const auto top = queue_.begin();
        if(top->first <= nowTime) {
            activeTimers_.push_back(top->second);
            queue_.erase(top);
            timers_.erase(top->second);
        }
        else {
            break;
        }
    }
}

void timerQueue::_reset(timeStamp nowTime)
{
    for(timer* it : activeTimers_) {
        if(it->isRepeat()) {
            it->reset(nowTime);
            queue_.insert({it->getTime(), it});
            timers_.insert(it);
        }
        else {
            delete it;
        }
    }
    timeStamp nextTime;
    if(!activeTimers_.empty()) {
        nextTime = activeTimers_.front()->getTime();
    }
    if(nextTime.valid()) {
        detail::setTimerfdTime(fd_, nextTime);
    }
    activeTimers_.clear();
}