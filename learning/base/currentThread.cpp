#include <learning/base/currentThread.h>
#include <learning/base/processInfo.h>

#include <execinfo.h>

namespace learning {
namespace currentThread {

thread_local pid_t threadId = 0;
thread_local std::string threadName = "unknown";

} // namespace currentThread

std::string currentThread::stackTrace()
{
    std::string stack;
    const int maxFrames = 200;
    void* frame[maxFrames];
    int nptrs = ::backtrace(frame, maxFrames);
    char** strings = ::backtrace_symbols(frame, nptrs);
    if(strings) {
        for(int i = 1; i != nptrs; ++i) {
            stack.append(strings[i]);
            stack.push_back('\n');
        }
        free(strings);
    }
    return stack;
}

} // namespace learning