#ifndef LEARNING_NET_HTTP_HTTPVERSION_H
#define LEARNING_NET_HTTP_HTTPVERSION_H

#include <learning/base/noncopyable.h>

namespace learning {
namespace net {

enum VERSION{
    HTTP10 = 10,
    HTTP11 = 11,
    HTTPUNKNOWN = 0
};

} // namespace net
} // namespace learning

#endif // LEARNING_NET_HTTP_HTTPVERSION_H