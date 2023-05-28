#ifndef LEARNING_BASE_PTHREAD_H
#define LEARNING_BASE_PTHREAD_H

#include <learning/base/noncopyable.h>
#include <learning/base/countDown.h>

#include <string>
#include <atomic>
#include <functional>
#include <pthread.h>
#include <condition_variable>

namespace learning {

class pthread: noncopyable
{
public:
    using threadFunc = std::function<void()>;
    pthread(threadFunc f, const std::string& name = "");
    ~pthread();

    void start();
    void join();

    bool isStarted() const
    { return started_; }
    bool isJoined() const
    { return joined_; }
    int count() const
    { return count_; }

private:
    void _setDefaultName()
    {
        name_ = "pthread" + std::to_string(count_);
    }

private:
    std::string name_;
    threadFunc threadFunc_;
    std::atomic_bool started_;
    std::atomic_bool joined_;
    pid_t tid_;
    pthread_t pt_;
    countDown latch_;

    static std::atomic_int count_;
};

} // namespace learning

#endif // LEARNING_BASE_PTHREAD_H