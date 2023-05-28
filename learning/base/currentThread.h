#ifndef LEARNING_BASE_CURRENTTHREAD_H
#define LEARNING_BASE_CURRENTTHREAD_H

#include <string>

namespace learning {
namespace currentThread {

extern thread_local pid_t threadId;
extern thread_local std::string threadName;
void cacheTid();

inline pid_t tid()
{
    if(__builtin_expect(threadId == 0, 0)) {
        cacheTid();
    }
    return threadId;
}

inline std::string tidToString()
{
    return std::to_string(threadId);
}

inline const std::string& threadname()
{ return threadName; }

std::string stackTrace();

} // namespace currentThread
} // namespace learning

#endif // LEARNING_BASE_CURRENTTHREAD_H