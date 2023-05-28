#include <learning/net/eventLoopThread.h>
#include <learning/net/eventLoop.h>

#include <assert.h>

using namespace learning;
using namespace learning::net;

eventLoopThread::eventLoopThread(threadInitCallback f, const std::string& name)
  : thread_(std::bind(&eventLoopThread::_threadFunc, this)),
    existing_(false),
    threadInitCallback_(f)
{}

/*  loop已对象的形式运行在thread中, quit之后, 会自己析构 */
eventLoopThread::~eventLoopThread()
{
    existing_ = false;
    if(loop_) {
        loop_->quit();
    }
}

eventLoop* eventLoopThread::startLoop()
{
    assert(!thread_.started());
    existing_ = true;
    thread_.start();
    {
        std::unique_lock<std::mutex> lock(mutex_);
        while(!loop_) {
            cond_.wait(lock);
        }
    }
    return loop_;
}

/*  thread运行loop */
void eventLoopThread::_threadFunc()
{
    assert(existing_);
    eventLoop loop;
    if(threadInitCallback_) {
        threadInitCallback_(&loop);
    }
    {
        std::unique_lock<std::mutex> lock(mutex_);
        loop_ = &loop;
        cond_.notify_one();
    }
    loop.loop();

    std::unique_lock<std::mutex> lock(mutex_);
    /*  为什么执着于置nullptr 
        防止后续通过loop_修改吗 */
    loop_ = nullptr;
}