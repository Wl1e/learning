#ifndef LEARNING_BASE_ASYNCLOGGING_H
#define LEARNING_BASE_ASYNCLOGGING_H

#include <learning/base/noncopyable.h>
#include <learning/base/mutex.h>
#include <learning/base/condition.h>
#include <learning/base/blockQueue.h>
#include <learning/base/thread.h>
#include <learning/base/fileUtil.h>
#include <learning/base/logStream.h>
#include <learning/base/countDown.h>

#include <memory>
#include <vector>
#include <atomic>
#include <mutex>
#include <condition_variable>

namespace learning
{
namespace base
{

class asyncLogging: noncopyable
{
public:
    asyncLogging(const std::string& name,
                 off_t rollSize,
                 int interval = 3);
    ~asyncLogging();

    void start();
    void stop();

    void append(const char* data, size_t len);

private:
    void _runInThread();

private:
    using buffer = detail::fixedBuf<detail::maxBufSize>;
    using bufferPtr = std::unique_ptr<buffer>;
    using bufferPtrVector = std::vector<bufferPtr>;

    std::string name_;
    std::atomic_bool isRunning_;
    off_t rollSize_;
    const int interval_;
    countDown latch_;
    thread thread_;
    std::mutex mutex_;
    bufferPtr currentBuf_;
    bufferPtr nextBuf_;
    bufferPtrVector buffers_;
    std::condition_variable cond_;


};

} // namespace base
} // namespace learning

#endif // LEARNING_BASE_ASYNCLOGGING_H