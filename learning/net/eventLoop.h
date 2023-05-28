#ifndef LEARNING_NET_EVENTLOOP_H
#define LEARNING_NET_EVENTLOOP_H

#include <learning/base/noncopyable.h>
#include <learning/base/timeStamp.h>
#include <learning/net/callback.h>
// #include <learning/base/mutex.h>

#include <mutex>
#include <vector>
#include <memory>
#include <atomic>
#include <any>

// using namespace learning::base;

namespace learning {
namespace net {

class poller;
class channel;
class timer;
class timerQueue;

class eventLoop
{
public:
    eventLoop();
    ~eventLoop();

    void loop();
    void quit();
    void updateChannel(channel* pChannel);
    void removeChannel(channel* pChannel);
    bool hasChannel(channel* pChannel);

    eventLoop* getEventLoop();

    bool isInLoopThread() const;
    void assertInLoopThread()
    {
        if(!isInLoopThread()) {
            _abortNotInLoopThread();
        }
    }

    using functor = std::function<void()>;
    void runInLoop(functor cb)
    {
        if(isInLoopThread()) {
            cb();
        }
        else {
            queueInLoop(cb);
        }
    }
    void queueInLoop(functor cb)
    {
        // mutexGuard guard(mutex_);
        std::lock_guard<std::mutex> guard(mutex_);
        pendingFunctions_.push_back(std::move(cb));

        if(!isInLoopThread() || handlePending_) {
            wakeup();
        }
    }

    timer* runAt(timerCallback f, timeStamp time);
    timer* runAfter(timerCallback f, double delay)
    { return runAt(f, timeStamp::now() + delay); }
    timer* runEvery(timerCallback f, double interval);

    void setContext(const std::any& context)
    { context_ = context; }
    const std::any& context() const
    { return context_; }
    std::any* contextPtr()
    { return &context_; }

    void wakeup();

private:
    void _abortNotInLoopThread();

    void _handelPendingFunctor();
    void _handleWakeup();
    void _printActiveChannels();

private:
    std::atomic_bool started_;
    std::atomic_bool quited_;
    pid_t tid_;

    std::unique_ptr<poller> poller_;
    std::unique_ptr<timerQueue> timers_;

    // mutex mutex_;
    std::mutex mutex_;
    timeStamp pollerRetTime;

    std::vector<channel*> activeChannel_;
    channel* currentChannel_;
    std::vector<timerCallback> pendingFunctions_;
    std::atomic_bool handleChannels_;
    std::atomic_bool handlePending_;
    std::any context_;

    int wakeFd_;
    std::unique_ptr<channel> wakeupChannel_;
};

} // namespace net
} //namespace learning

#endif // LEARNING_NET_EVENTLOOP_H