#ifndef LEARNING_BASE_LOGSTREAM_H
#define LEARNING_BASE_LOGSTREAM_H

#include <learning/base/stringPiece.h>
#include <learning/base/noncopyable.h>

namespace learning {

template<size_t N>
class fixedBuf
{
public:
    fixedBuf(): cur_(data_) {}
    ~fixedBuf() = default;

    void append(const std::string& data);
    size_t remain() const
    { return sizeof data_ - size(); }
    size_t size() const
    { return cur_ - &data_[0]; }
    bool empty() const
    { return size() == 0; }
    const char* getData() const
    { *cur_ = '\0'; return data_; }

private:
    char data_[N];
    char* cur_;
};

const size_t minBufSize = 4*1024;
const size_t maxBufSize = 40*1024;

class Fmt;

class logStream: noncopyable
{
public:
    logStream() = default;
    ~logStream() = default;

    using buffer = fixedBuf<minBufSize>;

    const buffer& getBuffer() const
    { return buf_; }

    using self = logStream;
    self& operator<<(short data);
    self& operator<<(int data);
    self& operator<<(int64_t data);
    self& operator<<(ushort data);
    self& operator<<(uint data);
    self& operator<<(uint64_t data);
    self& operator<<(float data);
    self& operator<<(double data);
    self& operator<<(char data);
    self& operator<<(const char* data);
    self& operator<<(const void* data);
    self& operator<<(const stringPiece& data);
    self& operator<<(const buffer& data);
    self& operator<<(const Fmt& data);

private:
    void _append(const std::string& data);

private:
    buffer buf_;
    const char* file_;
    int line_;
};

class Fmt
{
public:
    template<typename T>
    Fmt(const char* fmt, T len);
    const char* data() const
    { return data_; }
    size_t size() const
    { return len_; }

private:
    static constexpr int typeSize_ = 12;
    char data_[typeSize_];
    size_t len_;
};

} //namespace learning

#endif // LEARNING_BASE_LOGSTREAM_H