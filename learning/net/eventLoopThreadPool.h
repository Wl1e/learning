#ifndef LEARNING_NET_EVENTLOOPTHREADPOOL_H
#define LEARNING_NET_EVENTLOOPTHREADPOOL_H

#include <learning/base/noncopyable.h>

#include <vector>
#include <memory>
#include <functional>

namespace learning {
namespace net {

class eventLoop;
class eventLoopThread;

class eventLoopThreadPool: noncopyable
{
public:
    eventLoopThreadPool(eventLoop* baseLoop, const std::string& name = "eventLoopThreadPool");
    ~eventLoopThreadPool();

    using threadInitCallback = std::function<void(eventLoop*)>;
    void start(threadInitCallback cb = threadInitCallback());
    bool isStarted() const
    { return started_; }
    eventLoop* getNextLoop();
    void setThreadNums(size_t size)
    { threadCnt_ = size; }
    std::vector<eventLoop*> getAllLoop() const
    { return loops_; }
    const std::string& name() const
    { return name_; }

private:
    std::string name_;
    eventLoop* baseLoop_;
    std::vector<std::unique_ptr<eventLoopThread>> pool_;
    std::vector<eventLoop*> loops_;
    bool started_;
    size_t threadCnt_;
};

} // namespace net
} // namespace learning

#endif // LEARNING_NET_EVENTLOOPTHREADPOOL_H