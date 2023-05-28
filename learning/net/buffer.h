#ifndef LEARNING_NET_BUFFER_H
#define LEARNING_NET_BUFFER_H

#include <learning/base/noncopyable.h>
#include <learning/base/stringPiece.h>

#include <vector>
#include <string>
#include <assert.h>

namespace learning {
namespace net {

static const size_t defaultBufferSize = 1024;
static const size_t prependSize = 4;

class buffer: noncopyable
{
public:
    buffer(size_t size = defaultBufferSize)
      : readIndex_(prependSize),
        writeIndex_(prependSize)
    {
        buf_.resize(size);
    }
    ~buffer() = default;

    /*  for net */
    ssize_t readFd(int fd);
    std::string getLine();

    void swap(buffer& other)
    {
        std::swap(buf_, other.buf_);
        std::swap(readIndex_, other.readIndex_);
        std::swap(writeIndex_, other.writeIndex_);
    }
    const char* peek() const
    { return _begin() + readIndex_; }
    void ensureWrite(size_t size)
    {
        if(size > writeAbleBytes()) {
            _makeSpace(size);
        }
        assert(size <= writeAbleBytes());
    }
    void append(stringPiece data)
    {
        ensureWrite(data.size());
        std::copy(data.begin(), data.end(), beginWrite());
        hasWritten(data.size());

    }
    size_t readAbleBytes() const
    { return writeIndex_ - readIndex_; }
    size_t writeAbleBytes() const
    { return buf_.size() - writeIndex_; }
    void hasWritten(size_t size)
    {
        assert(size <= writeAbleBytes());
        writeIndex_ += size;
    }
    void receive(size_t len)
    {
        if(len > readAbleBytes()) {
            len = readAbleBytes();
        }
        readIndex_ += len;
    }
    void receiveAll()
    {
        receive(readAbleBytes());
    }
    std::string receiveAsString(size_t len)
    {
        if(len > readAbleBytes()) {
            len = readAbleBytes();
        }
        size_t oldReadIndex = readIndex_;
        receive(len);
        return std::string(_begin() + oldReadIndex, len);
    }
    std::string receiveAllAsString()
    {
        return receiveAsString(readAbleBytes());
    }
    void shrink()
    {
        size_t size = readAbleBytes();
        std::copy(peek(), beginWriteConst(), _begin() + prependSize);
        readIndex_ = prependSize;
        writeIndex_ = readIndex_ + size;
        buf_.resize(size > defaultBufferSize ? size : defaultBufferSize);
        buf_.shrink_to_fit();
    }
    const char* beginWriteConst() const
    { return _begin() + writeIndex_; }
    char* beginWrite()
    { return _begin() + writeIndex_; }

private:
    char* _begin()
    { return &buf_[0]; }
    const char* _begin() const
    { return &buf_[0]; }

    void _makeSpace(size_t size);

private:
    std::vector<char> buf_;
    size_t readIndex_;
    size_t writeIndex_;
};

} // namespace net
} // namespace learning

#endif // LEARNING_NET_BUFFER_H