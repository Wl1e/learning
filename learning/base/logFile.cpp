#include <learning/base/logFile.h>

using namespace learning;
using namespace learning::fileUtil;

const int secondPerDay = 24*60*60;

logFile::logFile(stringArg basename,
                 off_t maxRollSize,
                 int checkEveryN,
                 int flushInterval,
                 bool isSafe)
  : basename_(basename.data()),
    checkEveryN_(checkEveryN),
    maxRollSize_(maxRollSize),
    count_(0),
    flushInterval_(flushInterval),
    lastFlushTime_(0),
    lastCreateTime_(0),
    mutex_(isSafe ? std::make_unique<mutex>() : nullptr)
{
    assert(basename_.find('/') == std::string::npos);
    rollFile();
}

void logFile::append(stringPiece data)
{
    if(mutex_) {
        mutexGuard guard(*mutex_);
        appendWithoutGuard(data);
    }
    else {
        appendWithoutGuard(data);
    }
}

void logFile::append(const char* data, size_t len)
{
    append({data, len});
}

void logFile::rollFile()
{
    time_t nowTime = time(nullptr);
    nowTime = nowTime / secondPerDay * secondPerDay;
    if(nowTime > lastCreateTime_) {
        file_.reset(new writeFile(_fileName(nowTime)));
        count_ = 0;
        lastCreateTime_ = nowTime;
        lastFlushTime_ = nowTime;
    }
}

void logFile::flush()
{
    if(mutex_) {
        mutexGuard guard(*mutex_);
        flushWithoutGuard();
    }
    else {
        flushWithoutGuard();
    }
}

std::string logFile::_fileName(time_t time)
{
    tm t;
    t = *localtime(&time);
    char createTime[32];
    strftime(createTime, sizeof createTime, "_%Y-%m-%d_%H:%M:%S", &t);

    return basename_ + createTime + ".log";
}

void logFile::appendWithoutGuard(stringPiece data)
{
    file_->append(data);
    if(file_->writeBytes() >= maxRollSize_) {
        rollFile();
    }
    else {
        ++count_;
        if(count_ >= checkEveryN_) {
            time_t nowTime = time(nullptr);
            if(nowTime / secondPerDay * secondPerDay != lastCreateTime_) {
                rollFile();
            }
            else if((secondPerDay - lastFlushTime_) > 3) {
                flush();
            }
        }
    }
}
void logFile::flushWithoutGuard()
{
    lastFlushTime_ = time(nullptr);
    file_->fflush();
}