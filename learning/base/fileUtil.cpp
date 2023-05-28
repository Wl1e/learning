#include <learning/base/fileUtil.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace learning;
using namespace learning::fileUtil;

readFile::readFile(stringArg path)
  : fd_(::open(path.data(), O_RDONLY | O_CLOEXEC)),
    error_(0)
{
    if(fd_ < 0) {
        error_ = errno;
        char buf[64];
        char errorBuf[32];
        snprintf(buf,
                 sizeof buf,
                 "readFile: open error, error = %s",
                 strerror_r(error_, errorBuf, sizeof errorBuf));
        fputs(buf, stderr);
    }
}

std::string readFile::readToString(size_t size, int* pErrno)
{
    assert(pErrno);
    char data[size];
    size_t readSize = 0;
    while(readSize < size) {
        ssize_t n = ::read(fd_, data + readSize, sizeof data - readSize);
        if(n > 0) {
            readSize += n;
        }
        else {
            if(n != 0) {
                *pErrno = errno;
            }
            break;
        }
    }
    return data;
}
int readFile::readToBuffer(size_t size)
{
    int savedErrno = 0;
    size_t maxSize = sizeof data_ > size ? size : sizeof data_;
    size_t readSize = 0;
    while(readSize < maxSize) {
        ssize_t n = ::read(fd_, data_ + readSize, sizeof data_ - readSize);
        if(n > 0) {
            readSize += n;
        }
        else {
            if(n != 0) {
                savedErrno = errno;
            }
            break;
        }
    }
    return savedErrno;
}

std::string readFileToString(stringArg path, size_t maxSize, int* pErrno)
{
    return readFile(path).readToString(maxSize, pErrno);
}

int writeFile::append(stringPiece data)
{
    size_t writeSize = 0;
    int savedErrno = 0;
    while(writeSize < data.size()) {
        size_t n = fwrite(data.data() + writeSize, 1, data.size() - writeSize, fp_);
        if(n > 0) {
            writeSize += n;
        }
        else {
            break;
        }
    }
    writeBytes_ += writeSize;
    return savedErrno;
}

void writeFile::fflush()
{
    fflush_unlocked(fp_);
}