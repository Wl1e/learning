#ifndef LEARNING_NET_TCPCONNECTION_H
#define LEARNING_NET_TCPCONNECTION_H

#include <learning/base/noncopyable.h>
#include <learning/net/buffer.h>
#include <learning/net/callback.h>
#include <learning/net/inetAddress.h>
#include <learning/base/stringPiece.h>

#include <learning/base/timeStamp.h>

#include <memory>
#include <any>
#include <atomic>

namespace learning {

class timeStamp;

namespace net {

class eventLoop;
class channel;
class socket;

class tcpConnection: noncopyable,
                     public std::enable_shared_from_this<tcpConnection>
{
public:
    enum STATE {
        connecting,
        connected,
        disconnecting,
        disconnected
    };

    tcpConnection(const std::string& name,
                  eventLoop* loop,
                  int fd,
                  const inetAddress& localAddr,
                  const inetAddress& peerAddr);
    ~tcpConnection();

    const std::string& name() const
    { return name_; }
    eventLoop* loop() const
    { return loop_; }
    void setContext(const std::any& context)
    { context_ = context; }
    const std::any& context() const
    { return context_; }
    std::any* contextPtr()
    { return &context_; }
    STATE state() const
    { return connState_; }
    void setState(STATE state)
    { connState_ = state; }
    bool isConnected() const
    { return connState_ == connected; }

    void setConnectionCallback(const connectionCallback& cb)
    { connectionCallback_ = cb; }
    void setMessageCallback(const messageCallback& cb)
    { messageCallback_ = cb; }
    void setWriteCompleteCallback(const writeCompleteCallback& cb)
    { writeCompleteCallback_ = cb; }
    void setHighWaterMarkCallback(const highWaterMarkCallback& cb, size_t size)
    { highWaterMarkCallback_ = cb; highWaterMark_ = size; }
    void setCloseCallback(const closeCallback& cb)
    { closeCallback_ = cb; }

    void send(stringPiece data);
    void send(buffer* buf);

    void start();
    void stop();
    void shutdown();
    void forceClose();

    void ctorConnection();
    void dtorConnection();

private:
    void _handleMessage(timeStamp);
    void _handleWrite();
    void _handleClose();
    void _handleError();

    void _shutdownInLoop();
    void _forceCloseInLoop();

    void _sendInLoop(const char* data, size_t len);
    void _sendInLoop(stringPiece data);

    void _startReadInLoop();
    void _stopReadInLoop();

private:
    std::string name_;
    eventLoop* loop_;
    std::unique_ptr<channel> channel_;

    std::unique_ptr<socket> socket_;
    const inetAddress& localAddr_;
    const inetAddress& peerAddr_;

    std::any context_;
    std::atomic<STATE> connState_;
    bool reading_;

    connectionCallback connectionCallback_;
    messageCallback messageCallback_;
    writeCompleteCallback writeCompleteCallback_;
    closeCallback closeCallback_;
    highWaterMarkCallback highWaterMarkCallback_;
    size_t highWaterMark_;

    buffer inputBuffer_;
    buffer outputBuffer_;
};

} // namespace net
} // namespace learning

#endif // LEARNING_NET_TCPCONNECTION_H