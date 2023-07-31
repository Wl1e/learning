#include <learning/base/asyncLogging.h>
#include <learning/base/logFile.h>
#include <learning/base/timeStamp.h>

using namespace learning;
using namespace learning::base;
using namespace learning::detail;

asyncLogging::asyncLogging(const std::string& name,
                           off_t rollSize,
                           int interval)
  : name_(name),
    isRunning_(false),
    rollSize_(rollSize),
    interval_(interval),
    latch_(1),
    thread_(std::bind(&asyncLogging::_runInThread, this), "Logging"),
    mutex_(),
    currentBuf_(std::make_unique<buffer>()),
    nextBuf_(std::make_unique<buffer>()),
    cond_()
{
    currentBuf_->bzero();
    nextBuf_->bzero();
    buffers_.reserve(16);
}

asyncLogging::~asyncLogging()
{
    if(isRunning_) {
        stop();
    }
}

void asyncLogging::start()
{
    isRunning_ = true;
    latch_.wait();
    thread_.start();
}

void asyncLogging::stop()
{
    isRunning_ = false;
    cond_.notify_one();
    // thread_.join();
}

void asyncLogging::append(const char* data, size_t len)
{
    std::unique_lock<std::mutex> guard(mutex_);
    if(currentBuf_->remain() <= len) {
        buffers_.push_back(std::move(currentBuf_));
        if(nextBuf_) {
            currentBuf_ = std::move(nextBuf_);
        }
        else {
            currentBuf_.reset(new buffer);
        }
        currentBuf_->append(data, len);
        cond_.notify_one();
    }
    else {
        currentBuf_->append(data, len);
    }
}

void asyncLogging::_runInThread()
{
    assert(isRunning_);
    logFile output(name_, rollSize_, false);
    bufferPtr buffer1 = std::make_unique<buffer>();
    bufferPtr buffer2 = std::make_unique<buffer>();
    bufferPtrVector buffers;
    buffer1->bzero();
    buffer2->bzero();
    buffers.reserve(16);

    while(isRunning_) {
        {
            std::unique_lock<std::mutex> guard(mutex_);
            if(buffers_.empty()) {
                cond_.wait(guard, interval_);
            }
            buffers_.push_back(std::move(currentBuf_));
            currentBuf_ = std::move(buffer1);
            buffers.swap(buffers_);
            if(!nextBuf_) {
                nextBuf_ = std::move(buffer2);
            }
        }
        
        assert(!buffers.empty());
        if(buffers.size() > 25) {
            char buf[256];
            snprintf(buf, sizeof buf, "In asyncLogging::_runInThread buffers.size() is %ld, need a larger buffer", buffers.size());
            fputs(buf, stderr);
            output.append(buf, static_cast<int>(strlen(buf)));
            /*  为什么要删到只剩两个
                日志堆积, 直接丢弃   太粗暴了吧 */
            buffers.erase(buffers.begin() + 2, buffers.end());
        }

        for(const auto& data : buffers) {
            // use FILE or writev ? I don`t know
            output.append(data->data(), data->size());
        }
        /*  只留两个buffer给buffer1/2, 但是后面buffers会clear, 不需要resize吧 */
        if(buffers.size() > 2) {
            buffers.resize(2);
        }

        if(!buffer1) {
            assert(!buffers.empty());
            buffer1 = std::move(buffers.back());
            buffers.pop_back();
            buffer1.reset(new buffer);
        }
        if(!buffer2) {
            assert(!buffers.empty());
            buffer2 = std::move(buffers.back());
            buffers.pop_back();
            buffer2.reset(new buffer);
        }
        buffers.clear();
        output.flush();
    }
    output.flush();
}