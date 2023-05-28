#ifndef LEARNING_NET_ACCEPTOR_H
#define LEARNING_NET_ACCEPTOR_H

#include <learning/base/noncopyable.h>
#include <learning/net/socket.h>
#include <learning/net/channel.h>

#include <functional>

namespace learning {
namespace net {

class eventLoop;
class inetAddress;

class acceptor: noncopyable
{

public:
    acceptor(eventLoop* loop, const inetAddress& addr, bool reusePort);
    ~acceptor();

    void listen();
    bool isListening() const
    { return listening_; }

    using newConnectionCallback = std::function<void(int, const inetAddress&)>;
    void setNewConnectionCallback(const newConnectionCallback& cb)
    { newConnectionCallback_ = cb; }

private:
    void _handleRead();

private:
    eventLoop* loop_;
    socket fd_;
    channel channel_;
    newConnectionCallback newConnectionCallback_;
    bool listening_;
    int remainFd_;
};

} // namespace net
} // namespace learning

#endif // LEARNING_NET_ACCEPTOR_H