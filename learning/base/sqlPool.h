#ifndef LEARNING_BASE_SQLPOOL_H
#define LEARNING_BASE_SQLPOOL_H

#include <learning/base/noncopyable.h>
#include <learning/base/thread.h>
#include <learning/base/logging.h>

#include <mutex>
#include <vector>
#include <string>
#include <deque>
#include <condition_variable>
#include <pqxx/pqxx>

namespace learning {

class sqlPool: noncopyable
{
public:
    sqlPool(const std::string& name = "SQLPool");
    ~sqlPool();

    void setMaxPoolSize(size_t nums)
    { maxPoolSize_ = nums; }
    void setMaxTasksNums(size_t nums)
    { maxTaskNums_ = nums; }
    using task = std::function<void(const std::string&)>;
    using updateTask = std::function<pqxx::result(const std::string&)>;
    using threadInitCallback = thread::threadFunc;
    using threadTask = std::function<void(pqxx::connection* c)>;

    void add(const std::string& command)
    { addTask(std::bind(&sqlPool::_work, this, std::placeholders::_1, command)); }
    void del(const std::string& command)
    { addTask(std::bind(&sqlPool::_work, this, std::placeholders::_1, command)); }
    void update(const std::string& command)
    { addTask(std::bind(&sqlPool::_work, this, std::placeholders::_1, command)); }
    pqxx::result select(const std::string& command)
    { return _nontransaction(&useSelect_, command); }

    void addTask(threadTask Task)
    {
        if(maxPoolSize_ == 0) {
            task();
        }
        std::unique_lock<std::mutex> lock(mutex_);
        tasks_.push_back(std::move(Task));
        if(tasks_.size() >= maxTaskNums_) {
        }
        notEmpty_.notify_one();
    }
    

    const std::string& name() const
    { return name_; }

    void start();
    void stop();
    
    void setThreadInitCallback(const threadInitCallback& cb)
    { threadInitCallback_ = cb; }

private:
    void _runInThread(int i);

    pqxx::result _nontransaction(pqxx::connection* c, const std::string& command)
    {
        pqxx::nontransaction N(*c);
        return N.exec(command);
    }

    void _work(pqxx::connection* c, const std::string& command)
    {
      pqxx::work W(*c);
      W.exec(command);
      W.commit();
    }

private:
    std::string name_;
    size_t maxPoolSize_;
    size_t maxTaskNums_;
    std::vector<std::unique_ptr<thread>> pool_;
    std::mutex mutex_;
    std::deque<threadTask> tasks_;
    std::atomic_bool quited_;
    std::condition_variable notEmpty_;
    threadInitCallback threadInitCallback_;
    pqxx::connection useSelect_;
};

} // namespace learning

#endif // LEARNING_BASE_SQLPOOL_H