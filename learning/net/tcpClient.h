#ifndef LEARNING_NET_TCPCLIENT_H
#define LEARNING_NET_TCPCLIENT_H

#include <learning/base/noncopyable.h>
#include <learning/net/callback.h>

#include <atomic>
#include <memory>
#include <mutex>

namespace learning {
namespace net {

class eventLoop;
class inetAddress;
class connector;
using connectorPtr = std::shared_ptr<connector>;

class tcpClient: noncopyable
{
public:
    tcpClient(eventLoop* loop, const std::string& name, const inetAddress& addr);
    ~tcpClient();

    const std::string& name() const
    { return name_; }
    eventLoop* loop() const
    { return loop_; }
    bool isConnected() const
    { return connected_; }
    tcpConnectionPtr connection() const
    { return conn_; }

    void setConnectionCallback(const connectionCallback& cb)
    { connectionCallback_ = cb; }
    void setMessageCallback(const messageCallback& cb)
    { messageCallback_ = cb; }
    void setWriteCompleteCallback(const writeCompleteCallback& cb)
    { writeCompleteCallback_ = cb; }

    void connect();
    void disconnect();

    void stop();

private:
    void _newConnection(int fd);
    void _removeConnection(const tcpConnectionPtr& conn);

private:
    eventLoop* loop_;
    std::string name_;
    std::atomic_bool connected_;

    connectorPtr connector_;
    tcpConnectionPtr conn_;
    std::atomic_int connCnt_;
    std::mutex mutex_;

    connectionCallback connectionCallback_;
    messageCallback messageCallback_;
    writeCompleteCallback writeCompleteCallback_;
};

} // namespace net
} // namespace learning

#endif // LEARNING_NET_TCPCLIENT_H