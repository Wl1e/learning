#include <learning/base/threadPool.h>
#include <learning/base/logging.h>

using namespace learning;

threadPool::threadPool(const std::string& name)
  : name_(name),
    maxPoolSize_(0),
    quited_(false),
    threadInitCallback_(nullptr)
{}

threadPool::~threadPool()
{
    stop();
}

void threadPool::addTask(task Task)
{
    if(maxPoolSize_ == 0) {
        task();
    }
    std::unique_lock<std::mutex> lock(mutex_);
    tasks_.push_back(std::move(Task));
    if(tasks_.size() >= maxTaskNums_) {
        LOG_ERROR << "tasks is full";
    }
    notEmpty_.notify_one();
}

void threadPool::start()
{
    for(size_t i = 0; i != maxPoolSize_; ++i) {
        thread t(std::bind(&threadPool::_runInThread, this));
        pool_.push_back(std::unique_ptr<thread>(&t));
    }

    if(maxPoolSize_ == 0 && threadInitCallback_) {
        threadInitCallback_();
    }
}
void threadPool::stop()
{
    quited_ = true;
    notEmpty_.notify_all();
}