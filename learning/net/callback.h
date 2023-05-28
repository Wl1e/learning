#ifndef LEARNING_NET_CALLBACK_H
#define LEARNING_NET_CALLBACK_H

#include <functional>
#include <memory>

namespace learning{

class timeStamp;

namespace net{

class buffer;
class tcpConnection;

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

using tcpConnectionPtr = std::shared_ptr<tcpConnection>;
using messageCallback       = std::function<void(const tcpConnectionPtr&, buffer*, timeStamp)>;
using writeCompleteCallback = std::function<void(const tcpConnectionPtr&)>;
using connectionCallback    = std::function<void(const tcpConnectionPtr&)>;
using closeCallback         = std::function<void(const tcpConnectionPtr&)>;
using highWaterMarkCallback = std::function<void(const tcpConnectionPtr&, size_t)>;
using timerCallback         = std::function<void()>;

void defaultConnectionCallback(const tcpConnectionPtr&);
void defaultMessageCallback(const tcpConnectionPtr&, buffer*, timeStamp);

} // namespace net
} // namespace learning

#endif // LEARNING_NET_CALLBACK_H