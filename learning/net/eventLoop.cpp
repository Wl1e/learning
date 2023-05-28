#include <learning/net/eventLoop.h>
#include <learning/net/poller.h>
#include <learning/net/channel.h>
#include <learning/net/timerQueue.h>
#include <learning/base/mutex.h>
#include <learning/base/logging.h>

#include <sys/eventfd.h>
#include <signal.h>

using namespace learning;
using namespace learning::net;

namespace{

thread_local eventLoop* loopInThisThread = nullptr;
const int pollerTimeoutMS = 10000;

int createEventFd()
{
    int fd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if(fd < 0) {
        LOG_SYSFATAL << "createEventFd error";
        abort();
    }
    return fd;
}

class ignoreSigPipe
{
public:
    ignoreSigPipe()
    {
        signal(SIGPIPE, SIG_IGN);
        LOG_TRACE << "ignore SIGPIPE";
    }
};

} // namespace

eventLoop* eventLoop::getEventLoop()
{
     return loopInThisThread;
}

ignoreSigPipe ignoreSign;

eventLoop::eventLoop()
  : started_(false),
    quited_(false),
    tid_(currentThread::tid()),
    poller_(poller::defaultPoller(this)),
    timers_(new timerQueue(this)),
    pollerRetTime(),
    currentChannel_(nullptr),
    handleChannels_(false),
    handlePending_(false),
    wakeFd_(createEventFd()),
    wakeupChannel_(new channel(this, wakeFd_))
{
    LOG_DEBUG << "eventLoop created";
    if(loopInThisThread) {
        LOG_FATAL << "eventLoop exists";
    }
    else {
        loopInThisThread = this;
    }
    wakeupChannel_->setMessageCallback(std::bind(&eventLoop::_handleWakeup, this));
    wakeupChannel_->enableReading();
}

eventLoop::~eventLoop()
{
    LOG_DEBUG << "eventLoop of thread = " << tid_ << "destruces in thread " << currentThread::tid();
    wakeupChannel_->disableAll();
    wakeupChannel_->remove();
    close(wakeFd_);
    loopInThisThread = nullptr;
}

void eventLoop::loop()
{
    assert(!started_);
    started_ = true;
    quited_ = false;
    while(!quited_) {
        // int next = timers_->nextTime();
        activeChannel_.clear();
        pollerRetTime = poller_->poll(pollerTimeoutMS, &activeChannel_);
        if(logging::getLevel() <= logging::TRACE) {
            _printActiveChannels();
        }
        handleChannels_ = true;
        for(channel* pChannel : activeChannel_) {
            currentChannel_ = pChannel;
            currentChannel_->handle(pollerRetTime);
        }
        currentChannel_ = nullptr;
        handleChannels_ = false;

        _handelPendingFunctor();   
    }
    LOG_TRACE << "eventLoop stop";
    started_ = false;
}

void eventLoop::quit()
{
    quited_ = true;
    if(!isInLoopThread()) {
        wakeup();
    }
}

void eventLoop::updateChannel(channel* pChannel)
{
    assertInLoopThread();
    assert(pChannel->loop() == this);
    poller_->updateChannel(pChannel);
}
void eventLoop::removeChannel(channel* pChannel)
{
    assertInLoopThread();
    assert(pChannel->loop() == this);
    if(handleChannels_) {
        assert(currentChannel_ == pChannel || std::find(activeChannel_.begin(), activeChannel_.end(), pChannel) == activeChannel_.end());
    }
    poller_->removeChannel(pChannel);
}
bool eventLoop::hasChannel(channel* pChannel)
{
    return poller_->hasChannel(pChannel);
}

void eventLoop::_abortNotInLoopThread()
{
    LOG_FATAL << "not in loopThread, create tid = " << tid_
              << ", now tid = " << currentThread::tid();
}

void eventLoop::wakeup()
{
    int64_t n = 1;
    ssize_t ret = write(wakeFd_, &n, sizeof n);
    if(ret != sizeof n) {
        LOG_ERROR << "nums error";
    }
}
void eventLoop::_handleWakeup()
{
    int64_t n;
    ssize_t ret = read(wakeFd_, &n, sizeof n);
    if(ret != sizeof n) {
        LOG_ERROR << "nums error";
    }
}
bool eventLoop::isInLoopThread() const
{
    return tid_ == currentThread::tid();
}

timer* eventLoop::runAt(timerCallback f, timeStamp time)
{
    return timers_->add(time, 0, f);
}
timer* eventLoop::runEvery(timerCallback f, double interval)
{
    return timers_->add(timeStamp::now() + interval, interval, f);
}

void eventLoop::_handelPendingFunctor()
{
    handlePending_ = true;
    for(const auto& it : pendingFunctions_) {
        if(it) {
            it();
        }
    }
    handlePending_ = false;
}
void eventLoop::_printActiveChannels()
{
    for(const channel* p : activeChannel_) {
        LOG_TRACE << "{" << p->reventsToString() << "}";
    }
}