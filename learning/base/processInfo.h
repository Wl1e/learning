#ifndef LEARNING_BASE_PROCESSINFO_H
#define LEARNING_BASE_PROCESSINFO_H

#include <unistd.h>

namespace learning
{
namespace processInfo
{

pid_t gettid();

} // namespace processInfo
} //namespace learning

#endif // LEARNING_BASE_PROCESSINFO_H