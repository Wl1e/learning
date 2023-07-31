#ifndef LEARNING_BASE_BLOCKQUEUE_H
#define LEARNING_BASE_BLOCKQUEUE_H

#include <learning/base/noncopyable.h>
#include <learning/base/mutex.h>
#include <learning/base/condition.h>

#include <deque>

namespace learning
{
namespace base
{

template <typename T>
class blockQueue: noncopyable
{
public:
    blockQueue()
      : mutex_(),
        notEmpty_(mutex_),
        tasks_()
    {}

    void put(T& t)
    {
        mutexLockGuard guard(mutex_);
        tasks_.push_back(t);
        notEmpty_.notifyOne();
    }
    void put(T&& t)
    {
        mutexLockGuard guard(mutex_);
        tasks_.push_back(std::move(t));
        notEmpty_.notifyOne();
    }
    T take()
    {
        mutexLockGuard guard(mutex_);
        while(tasks_.empty()) {
            notEmpty_.wait();
        }
        assert(!tasks_.empty());
        T t = std::move(tasks_.front());
        tasks_.pop_front();
        return t;
    }
    size_t size() const
    {
        mutexLockGuard guard(mutex_);
        return tasks_.size();
    }

private:
    mutexLock mutex_;
    condition notEmpty_ GUARDED_BY(mutex_);
    std::deque<T> tasks_ GUARDED_BY(mutex_);
};

} // namespace base
} // namespace learning

#endif // LEARNING_BASE_BLOCKQUEUE_H