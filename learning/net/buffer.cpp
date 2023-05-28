#include <learning/base/logging.h>
#include <learning/net/buffer.h>

#include <unistd.h>
#include <sys/uio.h>

using namespace learning;
using namespace learning::net;

const char CRLF[]{"\r\n"};

size_t defaultResize = 128;

ssize_t buffer::readFd(int fd)
{
    assert(fd > 0);
    const int iovCnt = 2;
    iovec iov[iovCnt];
    char data[65536];
    iov[0].iov_base = beginWrite();
    iov[0].iov_len = writeAbleBytes();
    iov[1].iov_base = data;
    iov[1].iov_len = sizeof data;
    ssize_t n = readv(fd, iov, iovCnt);
    if(n < 0) {
        LOG_ERROR << "read error, fd = " << fd
                    << ", error is " << errorString(errno);
    }
    else {
        if(static_cast<size_t>(n) < iov[0].iov_len) {
            hasWritten(n);
        }
        else {
            hasWritten(iov[0].iov_len);
            append({data, n - iov[0].iov_len});
        }
    }
    return n;
}

std::string buffer::getLine()
{
    std::string ret;
    if(readAbleBytes() != 0)
    {
        const char* it = std::search(peek(), beginWriteConst(), CRLF, CRLF+2);
        ret = receiveAsString(it - peek());
        // 找到了/r/n, 但是it只到/r, 这里把/r/n也receive掉
        if(*it == '\r' && readAbleBytes() >= 2)
            receive(2);
    }
    return ret;
}

void buffer::_makeSpace(size_t size)
{
    if(size + readAbleBytes() + prependSize <= buf_.size()) {
        shrink();
    }
    else {
        size_t oldSize = buf_.size();
        buf_.resize(oldSize + (defaultResize > size ? defaultResize : size));
    }
}
