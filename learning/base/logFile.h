#ifndef LEARNING_BASE_LOGFILE_H
#define LEARNING_BASE_LOGFILE_H

#include <learning/base/noncopyable.h>
#include <learning/base/fileUtil.h>
#include <learning/base/mutex.h>

#include <memory>

namespace learning{

class logFile: noncopyable
{
public:
    logFile(stringArg basename,
            off_t maxRollSize,
            int maxCheckSize = 1024,
            int flushInterval = 3,
            bool isSafe = false);
    ~logFile() = default;

    void append(stringPiece data);
    void append(const char* data, size_t len);
    void rollFile();
    void flush();

private:
    std::string _fileName(time_t time);

    void appendWithoutGuard(stringPiece data);
    void flushWithoutGuard();

private:
    std::string basename_;
    int checkEveryN_;
    off_t maxRollSize_;
    int count_;
    int flushInterval_;
    time_t lastFlushTime_;
    time_t lastCreateTime_;
    std::unique_ptr<mutex> mutex_;
    std::unique_ptr<fileUtil::writeFile> file_;
};

} // namespace learning

#endif // LEARNING_BASE_LOGFILE_H
