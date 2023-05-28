#include <learning/base/logStream.h>

#include <assert.h>

using namespace learning;

template<size_t N>
void fixedBuf<N>::append(const std::string& data)
{
    assert(data.size() < remain());
    cur_ = std::copy(data.begin(), data.end(), cur_);
}
template class fixedBuf<minBufSize>;
template class fixedBuf<maxBufSize>;

template<typename T>
Fmt::Fmt(const char* data, T len)
  : len_(0)
{
    static_assert(std::is_arithmetic_v<T> == true, "Fmt: T must be arithmetic type");
    if(data) {
        snprintf(data_, sizeof data_, data, len);
    }
    len_ = static_cast<size_t>(len);
    assert(sizeof data_ > len_);
}

logStream::self& logStream::operator<<(short data)
{
    return operator<<(Fmt("%d", data));
}
logStream::self& logStream::operator<<(int data)
{
    return operator<<(Fmt("%d", data));
}
logStream::self& logStream::operator<<(int64_t data)
{
    return operator<<(Fmt("%ld", data));
}
logStream::self& logStream::operator<<(ushort data)
{
    return operator<<(Fmt("%u", data));
}
logStream::self& logStream::operator<<(uint data)
{
    return operator<<(Fmt("%u", data));
}
logStream::self& logStream::operator<<(uint64_t data)
{
    return operator<<(Fmt("%lu", data));
}
logStream::self& logStream::operator<<(float data)
{
    return operator<<(Fmt("%f", data));
}
logStream::self& logStream::operator<<(double data)
{
    return operator<<(Fmt("%f", data));
}
logStream::self& logStream::operator<<(char data)
{
    _append({&data, 1});
    return *this;
}
logStream::self& logStream::operator<<(const char* data)
{
    _append(data);
    return *this;
}
logStream::self& logStream::operator<<(const void* data)
{
    return operator<<(static_cast<const char*>(data));
}
logStream::self& logStream::operator<<(const stringPiece& data)
{
    _append(std::string(data.begin(), data.size()));
    return *this;
}
logStream::self& logStream::operator<<(const buffer& data)
{
    _append(data.getData());
    return *this;
}
logStream::self& logStream::operator<<(const Fmt& data)
{
    _append(data.data());
    return *this;
}

void logStream::_append(const std::string& data)
{
    buf_.append(data);
}

template Fmt::Fmt(const char* fmt, short);
template Fmt::Fmt(const char* fmt, ushort);
template Fmt::Fmt(const char* fmt, int);
template Fmt::Fmt(const char* fmt, uint);
template Fmt::Fmt(const char* fmt, int64_t);
template Fmt::Fmt(const char* fmt, uint64_t);
template Fmt::Fmt(const char* fmt, float);
template Fmt::Fmt(const char* fmt, double);