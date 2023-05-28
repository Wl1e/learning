#ifndef LEARNING_NET_EVENTLOOPTHREAD_H
#define LEARNING_NET_EVENTLOOPTHREAD_H

#include <learning/base/noncopyable.h>
#include <learning/base/thread.h>

#include <mutex>
#include <condition_variable>

namespace learning {
namespace net {

class eventLoop;

class eventLoopThread: noncopyable
{
public:
    using threadInitCallback = std::function<void(eventLoop*)>;
    eventLoopThread(threadInitCallback f, const std::string& name = "");
    ~eventLoopThread();

    eventLoop* startLoop();

private:
    void _threadFunc();

private:
    eventLoop* loop_;
    std::mutex mutex_;
    thread thread_;
    std::atomic_bool existing_;
    threadInitCallback threadInitCallback_;
    std::condition_variable cond_;
};

} // namespace net
} // namespace learning

#endif // LEARNING_NET_EVENTLOOPTHREAD_H