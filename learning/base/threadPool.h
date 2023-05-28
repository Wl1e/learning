#ifndef LEARNING_BASE_THREADPOOL_H
#define LEARNING_BASE_THREADPOOL_H

#include <learning/base/noncopyable.h>
#include <learning/base/thread.h>

#include <mutex>
#include <vector>
#include <string>
#include <deque>
#include <condition_variable>

namespace learning {

class threadPool: noncopyable
{
public:
    threadPool(const std::string& name = "threadPool");
    ~threadPool();

    void setMaxPoolSize(size_t nums)
    { maxPoolSize_ = nums; }
    void setMaxTasksNums(size_t nums)
    { maxTaskNums_ = nums; }
    using task = thread::threadFunc;
    void addTask(task Task);

    const std::string& name() const
    { return name_; }

    void start();
    void stop();
    
    void setThreadInitCallback(const task& threadInitCallback)
    { threadInitCallback_ = threadInitCallback; }

private:
    void _runInThread()
    {
        if(threadInitCallback_) {
            threadInitCallback_();
        }

        std::unique_lock<std::mutex> lock(mutex_);
        while(!quited_) {
            while(tasks_.empty()) {
                notEmpty_.wait(lock);
            }
            if(!tasks_.empty()) {
                task it = std::move(tasks_.front());
                tasks_.pop_back();
                it();
            }
        }
    }

private:
    std::string name_;
    size_t maxPoolSize_;
    size_t maxTaskNums_;
    std::vector<std::unique_ptr<thread>> pool_;
    std::mutex mutex_;
    std::deque<task> tasks_;
    std::atomic_bool quited_;
    std::condition_variable notEmpty_;
    task threadInitCallback_;
};

} // namespace learning

#endif // LEARNING_BASE_THREADPOOL_H