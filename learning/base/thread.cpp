#include <learning/base/thread.h>
#include <learning/base/logging.h>
#include <learning/base/currentThread.h>
#include <learning/base/processInfo.h>

#include <exception>

using namespace learning;

std::atomic_int thread::count_ = 0;

void currentThread::cacheTid()
{
    if(threadId == 0) {
        threadId = learning::processInfo::gettid();
    }
}

void threadF(const std::string& threadName, thread::threadFunc f)
{

    try {
        f();
    }
    catch(const std::exception& x) {
        LOG_ERROR << "f() error, x = " << x.what()
                  << ", tid = " << currentThread::tid()
                  << ", threadName = " << threadName;
    }
}

thread::thread(threadFunc f, const std::string& name)
  : f_(f),  
    started_(false)
{
    ++count_;
    name_ = name.size() == 0 ? _setDefaultName() : name;
}

void thread::start()
{
    started_ = true;
    thread_ = std::thread(threadF, name_, f_);
    thread_.detach();
}