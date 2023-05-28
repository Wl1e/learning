#include <learning/base/processInfo.h>

using namespace learning;

pid_t processInfo::gettid()
{
    return ::gettid();
}