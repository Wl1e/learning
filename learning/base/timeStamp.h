#ifndef LEARNING_BASE_TIMESTAMP_H
#define LEARNING_BASE_TIMESTAMP_H

#include <chrono>
#include <string>

#include <learning/base/copyable.h>

namespace learning{

using Clock = std::chrono::high_resolution_clock;
using MS = std::chrono::milliseconds;
using NS = std::chrono::nanoseconds;
using TimeStamp = std::chrono::time_point<Clock, MS>;
using std::chrono::duration_cast;

class timeStamp: public copyable
{
public:
    timeStamp(MS time_since_epoch = MS::zero())
      : time_(time_since_epoch) {}
    explicit timeStamp(TimeStamp t)
      : time_(t) {}
    ~timeStamp() = default;

    static const int MSPerSecond = 1000;
    static timeStamp now()
    { return duration_cast<MS>(Clock::now().time_since_epoch()); }
    static timeStamp invalid()
    { return timeStamp(MS::zero()); }
    TimeStamp getTimeStamp() const
    { return time_; }
    int64_t MSSinceEpoch() const
    { return duration_cast<MS>(time_.time_since_epoch()).count(); }
    bool valid() const
    { return time_.time_since_epoch() != MS::zero(); }
    std::string formatToString() const
    {
        char buf[64];
        time_t tim = std::chrono::system_clock::to_time_t(time_);

        strftime(buf, sizeof buf, "%Y_%m_%d %H:%M:%S", localtime(&tim));
        return buf;
    }
    timeStamp operator+(double interval)
    {
        return timeStamp(time_ + static_cast<MS>(static_cast<int64_t>(interval*MSPerSecond)));
    }
    void operator+=(double interval)
    {
        time_ + static_cast<MS>(static_cast<int64_t>(interval*MSPerSecond));
    }
    // bool operator>(const timeStamp& t) {
    //     return getTimeStamp() > t.getTimeStamp();
    // }
    // bool operator<(const timeStamp& t) {
    //     return getTimeStamp() < t.getTimeStamp();
    // }
    // bool operator>=(const timeStamp& t) {
    //     return getTimeStamp() >= t.getTimeStamp();
    // }
    // bool operator<=(const timeStamp& t) {
    //     return getTimeStamp() <= t.getTimeStamp();
    // }
    // bool operator==(const timeStamp& t) {
    //     return getTimeStamp() == t.getTimeStamp();
    // }
    // bool operator!=(const timeStamp& t) {
    //     return getTimeStamp() != t.getTimeStamp();
    // }

private:
    TimeStamp time_;
};

/*  多重定义了 */
#define CAMP(cmp) \
inline bool operator cmp(const timeStamp& lhs, const timeStamp& rhs) { \
    return lhs.getTimeStamp() cmp rhs.getTimeStamp(); \
}

CAMP(>)
CAMP(<)
CAMP(>=)
CAMP(<=)
CAMP(==)
CAMP(!=)

inline MS operator-(const timeStamp& lhs, const timeStamp& rhs)
{
    return duration_cast<MS>(lhs.getTimeStamp() - rhs.getTimeStamp());
}

} // namespace learning

#endif // LEARNING_BASE_TIMESTAMP_H