#include <learning/base/pthread.h>
#include <learning/base/currentThread.h>
#include <learning/base/logging.h>

#include <assert.h>
#include <exception>

using namespace learning;

std::atomic_int pthread::count_ = 0;

class threadData
{
public:
    threadData(pthread::threadFunc f,
               std::string threadName,
               pid_t* pTid,
               countDown* pLatch)
      : f_(f),
        threadName_(threadName),
        pTid_(pTid),
        pLatch_(pLatch)
    {}

    void threadFunc()
    {
        *pTid_ = currentThread::tid();
        pTid_ = nullptr;
        pLatch_->downCount();
        pLatch_ = nullptr;
        
        try {
            f_();
            currentThread::threadName = "finished";
        }
        catch(const std::exception& x) {
            currentThread::threadName = "crashed";
            LOG_ERROR << "cb() error, x = " << x.what()
                    << ", tid = " << currentThread::tid()
                    << ", threadName = " << threadName_;
            throw(x);
        }
    }

private:
    pthread::threadFunc f_;
    std::string threadName_;
    pid_t* pTid_;
    countDown* pLatch_;
};

void* threadF(void* ptr)
{
    threadData* pData = static_cast<threadData*>(ptr);
    pData->threadFunc();
    return nullptr;
}

pthread::pthread(threadFunc cb, const std::string& name)
  : threadFunc_(std::move(cb)),
    started_(false),
    joined_(false),
    tid_(0),
    pt_(0),
    latch_(1)
{
    ++count_;
    if(name.size() == 0) {
        _setDefaultName();
    }
    else {
        name_ = name;
    }
}

pthread::~pthread()
{
    if(started_ && !joined_) {
        pthread_detach(pt_);
    }
}

void pthread::start()
{
    assert(!started_);
    threadData mdata{threadFunc_, name_, &tid_, &latch_};
    pthread_create(&pt_, nullptr, threadF, &mdata);
    latch_.wait();
}

void pthread::join()
{
    assert(started_);
    assert(!joined_);
    pthread_join(pt_, nullptr);
    joined_ = true;
}
