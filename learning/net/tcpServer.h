#ifndef LEARNING_NET_TCPSERVER_H
#define LEARNING_NET_TCPSERVER_H

#include <learning/base/noncopyable.h>
#include <learning/net/tcpConnection.h>
#include <learning/net/callback.h>
#include <learning/net/inetAddress.h>

#include <memory>
#include <map>
#include <atomic>

namespace learning {
namespace net {

class eventLoop;
class acceptor;
class eventLoopThreadPool;

class tcpServer: noncopyable
{
public:
    tcpServer(eventLoop* loop,
              const std::string& name,
              const inetAddress& addr,
              bool reusePort);
    ~tcpServer();
    
    void start();

    const std::string& name() const
    { return name_; }
    eventLoop* loop() const
    { return loop_; }
    std::shared_ptr<eventLoopThreadPool> loopPool() const
    { return loopPool_; }
    size_t connCnt() const
    { return connCnt_; }

    void setThreadCnt(size_t n);
    using threadInitCallback = std::function<void(eventLoop*)>;
    void setThreadInitCallback(const threadInitCallback& cb)
    { threadInitCallback_ = cb; }
    void setMessageCallback(const messageCallback& cb)
    { messageCallback_ = cb; }
    void setWriteCompleteCallback(const writeCompleteCallback& cb)
    { writeCompleteCallback_ = cb; }
    void setConnectionCallback(const connectionCallback& cb)
    { connectionCallback_ = cb; }

private:
    void _newConn(int fd, const inetAddress& addr);
    void _removeConn(const tcpConnectionPtr& conn);
    void _removeConnInLoop(const tcpConnectionPtr& conn);

private:
    std::string name_;
    eventLoop* loop_;
    bool started_;
    std::unique_ptr<acceptor> acceptor_;
    using connMap = std::map<std::string, tcpConnectionPtr>;
    int connCnt_;
    connMap conns_;
    std::shared_ptr<eventLoopThreadPool> loopPool_;

    connectionCallback connectionCallback_;
    messageCallback messageCallback_;
    writeCompleteCallback writeCompleteCallback_;
    threadInitCallback threadInitCallback_;
};

} // namespace net
} // namespace learning

#endif // LEARNING_NET_TCPSERVER_H