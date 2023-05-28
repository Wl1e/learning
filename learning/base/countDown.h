#ifndef LEARNING_BASE_COUNTDOWN_H
#define LEARNING_BASE_COUNTDOWN_H

#include <learning/base/noncopyable.h>

#include <mutex>
#include <condition_variable>

namespace learning {

class countDown: noncopyable
{
public:
    countDown(int i = 0)
      : count_(i)
    {}
    ~countDown() = default;

    void wait()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        while(count_ > 0) {
            cond_.wait(lock);
        }
    }
    void downCount()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        --count_;
        if(count_ == 0) {
            cond_.notify_all();
        }
    }
    int count()
    {
        std::unique_lock<std::mutex> guard(mutex_);
        return count_;
    }

private:
    int count_;
    std::mutex mutex_;
    std::condition_variable cond_;
};

} // namespace learning

#endif // LEARNING_BASE_COUNTDOWN_H