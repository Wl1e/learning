#ifndef LEARNING_BASE_THREAD_H
#define LEARNING_BASE_THREAD_H

#include <learning/base/noncopyable.h>

#include <string>
#include <thread>
#include <functional>
#include <atomic>

namespace learning {

class thread: noncopyable
{
public:
    using threadFunc = std::function<void()>;
    thread(threadFunc f, const std::string& name = "");
    ~thread() = default;
    /*  默认detach */
    void start();
    bool started() const
    { return started_; }

private:
    std::string _setDefaultName()
    {
        return "thread" + std::to_string(count_);
    }

private:
    std::string name_;
    std::thread thread_;
    threadFunc f_;

    std::atomic_bool started_;
    static std::atomic_int count_;
};

} // namespace learning

#endif // LEARNING_BASE_THREAD_H