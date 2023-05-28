#ifndef LEARNING_NET_CONNECTOR_H
#define LEARNING_NET_CONNECTOR_H

#include <learning/base/noncopyable.h>
#include <learning/net/socket.h>
#include <learning/net/inetAddress.h>

#include <memory>
#include <functional>

namespace learning {
namespace net {

class eventLoop;
class channel;

class connector: noncopyable,
                 public std::enable_shared_from_this<connector>
{
public:
    connector(eventLoop* loop, const inetAddress& addr);
    ~connector();

    void start();
    void restart();
    void stop();

    using newConnectionCallback = std::function<void(int)>;
    void setNewConnectionCallback(const newConnectionCallback& cb)
    { newConnectionCallback_ = cb; }

    const inetAddress& addr() const
    { return addr_; }

    enum STATE{ connecting, connected, disconnected };

private:
    void _setState(STATE state)
    { state_ = state; }
    void _runInLoop();
    void _stopInLoop();
    void _connect();
    void _connecting(int fd);
    void _handleError();
    void _handleWrite();
    int _removeAndResetChannel();
    void _resetChannel();
    void _retry(int fd);

private:
    eventLoop* loop_;
    std::unique_ptr<channel> channel_;
    inetAddress addr_;
    bool started_;
    STATE state_;
    newConnectionCallback newConnectionCallback_;
    int retryIntervalMS_;
};

} // namespace net
} // namespace learning

#endif // LEARNING_NET_CONNECTOR_H