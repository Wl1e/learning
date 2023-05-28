#include <learning/net/eventLoopThreadPool.h>
#include <learning/net/eventLoopThread.h>
#include <learning/net/eventLoop.h>

#include <assert.h>

using namespace learning;
using namespace learning::net;

    std::string name_;
    eventLoop* baseLoop_;
    std::vector<std::unique_ptr<eventLoopThread>> pool_;
    std::vector<eventLoop*> loops_;
    bool started_;
    int nextLoop_;
    size_t threadCnt_;

eventLoopThreadPool::eventLoopThreadPool(eventLoop* baseLoop, const std::string& name)
  : name_(name),
    baseLoop_(baseLoop),
    started_(false),
    threadCnt_(0)
{}
eventLoopThreadPool::~eventLoopThreadPool() = default;

void eventLoopThreadPool::start(threadInitCallback cb)
{
    assert(!started_);
    baseLoop_->assertInLoopThread();
    started_ = true;
    for(size_t i{0}; i != threadCnt_; ++i) {
        eventLoopThread* eLT = new eventLoopThread(cb, "eventLoopThread" + std::to_string(i + 1));
        loops_.push_back(eLT->startLoop());
    }
    if(threadCnt_ == 0 && cb) {
        cb(baseLoop_);
    }
}

eventLoop* eventLoopThreadPool::getNextLoop()
{
    baseLoop_->assertInLoopThread();
    assert(started_);
    static size_t next = 0;
    eventLoop* loop = baseLoop_;
    if(!loops_.empty()) {
        loop = loops_[next];
        ++next;
        /*  这里可以看出不能简单换成atomic_int, 可以像threadPool一样换成锁, 效率如何? */
        if(next >= loops_.size()) {
            next = 0;
        }
    }
    return loop;
}
