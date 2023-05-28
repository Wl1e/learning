#ifndef LEARNING_BASE_FILEUTIL_H
#define LEARNING_BASE_FILEUTIL_H

#include <learning/base/noncopyable.h>
#include <learning/base/stringPiece.h>

#include <string>
#include <unistd.h>
#include <assert.h>

namespace learning{
namespace fileUtil{

static const size_t MIN_BUF_SIZE = 4*1024;
static const size_t MAX_BUF_SIZE = 64*1024;

class readFile: noncopyable
{
public:
    explicit readFile(stringArg path);
    ~readFile()
    {
        if(fd_ > 0) {
            ::close(fd_);
        }
    }

    std::string readToString(size_t size, int* pErrno);
    int readToBuffer(size_t size);
    const char* buffer() const
    { return data_; }
    int opened() const
    { return error_; }

private:
    int fd_;
    char data_[MAX_BUF_SIZE];
    int error_;
};

std::string readFileToString(stringArg path, size_t maxSize, int* pErrno);

class writeFile: noncopyable
{
public:
    explicit writeFile(stringArg path)
      : fp_(::fopen(path.data(), "a"))
    {
        assert(fp_);
        setbuffer(fp_, buf_, sizeof buf_);
    }
    ~writeFile()
    {
        ::fclose(fp_);
    }

    int append(stringPiece data);
    void fflush();
    off_t writeBytes() const
    { return writeBytes_; }

private:
    FILE* fp_;
    char buf_[MAX_BUF_SIZE];
    off_t writeBytes_;
};

} // namespace fileUtil
} // namespace learning

#endif // LEARNING_BASE_FILEUTIL_H