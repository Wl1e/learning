#include <learning/net/tcpConnection.h>
#include <learning/net/eventLoop.h>
#include <learning/net/channel.h>
#include <learning/net/socket.h>
#include <learning/base/logging.h>
#include <learning/net/socketOps.h>

#include <assert.h>

using namespace learning;
using namespace learning::net;

void learning::net::defaultConnectionCallback(const tcpConnectionPtr& conn)
{
    printf("why\n");
    LOG_TRACE << "a new conn, name = " << conn->name();
}
void learning::net::defaultMessageCallback(const tcpConnectionPtr& conn, buffer* buf, timeStamp t)
{
    LOG_DEBUG << "conn[" << conn->name() << "]"
              << " data = " << buf->receiveAllAsString()
              << ", time = " << t.formatToString();
    buf->receiveAll();
}

tcpConnection::tcpConnection(const std::string& name,
                eventLoop* loop,
                int fd,
                const inetAddress& localAddr,
                const inetAddress& peerAddr)
  : name_(name),
    loop_(loop),
    channel_(new channel(loop_, fd)),
    socket_(new socket(fd)),
    localAddr_(localAddr),
    peerAddr_(peerAddr),
    connState_(connecting),
    reading_(channel_->isReading()),
    highWaterMark_(60*1024*1024)
{
    channel_->setCloseCallback(std::bind(&tcpConnection::_handleClose, this));
    channel_->setMessageCallback(std::bind(&tcpConnection::_handleMessage, this, _1));
    channel_->setWriteCallback(std::bind(&tcpConnection::_handleWrite, this));
    channel_->setErrorCallback(std::bind(&tcpConnection::_handleError, this));
    LOG_DEBUG << "a new connection, fd = " << fd;
    socket_->setKeepAlive(true);
}
tcpConnection::~tcpConnection()
{
    LOG_DEBUG << "a connection close, name = " << name_
              << ", fd = " << socket_->fd()
              << ", state = " << connState_;
    assert(connState_ == disconnected);
}

void tcpConnection::start()
{
    loop_->runInLoop(std::bind(&tcpConnection::_startReadInLoop, this));
}
void tcpConnection::stop()
{
    assert(connState_ == connected);
    channel_->disableReading();
    loop_->runInLoop(std::bind(&tcpConnection::_stopReadInLoop, this));
    connState_ = disconnected;
}

void tcpConnection::_startReadInLoop()
{
    loop_->assertInLoopThread();
    assert(connState_ == connecting);
    if(!reading_ || !channel_->isReading()) {
        channel_->enableReading();
        reading_ = channel_->isReading();
    }
}
void tcpConnection::_stopReadInLoop()
{
    loop_->assertInLoopThread();
    assert(connState_ == connected);
    if(reading_ || channel_->isReading()) {
        channel_->disableReading();
        reading_ = channel_->isReading();
    }
}

void tcpConnection::send(stringPiece data)
{
    if(connState_ == connected) {
        if(loop_->isInLoopThread()) {
            _sendInLoop(data);
        }
        else {
            void(tcpConnection::*fp)(const stringPiece) = &tcpConnection::_sendInLoop;
            loop_->runInLoop(std::bind(fp,
                                       shared_from_this(),
                                       std::forward<std::string_view>(data)));
        }
    }
}
void tcpConnection::send(buffer* buf)
{
    if(connState_ == connected) {
        buffer newBuf;
        newBuf.swap(*buf);
        send(newBuf.receiveAllAsString());
    }
}
void tcpConnection::_sendInLoop(const char* data, size_t len)
{
    loop_->assertInLoopThread();
    if(connState_ != connected) {
        LOG_ERROR << "connection close";
        return;
    }

    size_t remain = len;
    bool error = false;
    if(!channel_->isWriting() && outputBuffer_.readAbleBytes() == 0) {
        ssize_t n = sockets::write(socket_->fd(),
                                    data,
                                    len);
        if(n >= 0) {
            remain -= n;
            if(n == 0 && writeCompleteCallback_) {
                loop_->queueInLoop(std::bind(writeCompleteCallback_, shared_from_this()));
            }
        }
        else {
            if(errno != EWOULDBLOCK) {
                if(errno == EPIPE || errno == ECONNRESET) {
                    error = true;
                }
                LOG_SYSERR << "write error";
                /*  写一半, 前面全丢掉 */
                remain = len;
            }
        }
    }

    if(!error && remain > 0) {
        size_t outBufSize = outputBuffer_.writeAbleBytes();
        if(highWaterMarkCallback_ && outputBuffer_.readAbleBytes() + remain >= highWaterMark_) {
            loop_->queueInLoop(std::bind(highWaterMarkCallback_, shared_from_this(), outBufSize + remain));
        }
        outputBuffer_.append({data+(len-remain), remain});
        if(!channel_->isWriting()) {
            channel_->enableWriting();
        }
    }
}
void tcpConnection::_sendInLoop(stringPiece data)
{
    _sendInLoop(data.data(), data.size());
}

void tcpConnection::_handleMessage(timeStamp t)
{
    loop_->assertInLoopThread();
    assert(connState_ == connected);
    ssize_t n = inputBuffer_.readFd(socket_->fd());
    if(n > 0) {
        messageCallback_(shared_from_this(), &inputBuffer_, t);
    }
    else if(n == 0) {
        _handleClose();
    }
    else {
        LOG_SYSERR << "buffer::readFd error, connName = " << name_
                   << ", fd = " << socket_->fd();
    }
}
void tcpConnection::_handleWrite()
{
    loop_->assertInLoopThread();
    assert(connState_ == connected);
    if(channel_->isWriting()) {
        ssize_t n = sockets::write(socket_->fd(),
                                   outputBuffer_.peek(),
                                   outputBuffer_.readAbleBytes());
        if(n > 0) {
            outputBuffer_.hasWritten(n);
            channel_->disableWriting();
            if(writeCompleteCallback_) {
                loop_->queueInLoop(std::bind(writeCompleteCallback_, shared_from_this()));
            }
            if(connState_ == disconnecting) {
                setState(disconnected);
            }
        }
        else if(n == 0) {
            _shutdownInLoop();
        }
        else {
            LOG_ERROR << "write error, connName = " << name_
                      << ", fd = " << socket_->fd();
            _shutdownInLoop();
        }
    }
    else {
        LOG_DEBUG << "channel cannt write, fd = " << channel_->fd();
    }
}
void tcpConnection::_handleClose()
{
    loop_->assertInLoopThread();
    LOG_TRACE << "close connection, fd = " << socket_->fd();
    assert(connState_ == connected || connState_ == disconnecting);
    channel_->disableAll();

    tcpConnectionPtr guard(shared_from_this());
    connectionCallback_(guard);
    closeCallback_(guard);
}
void tcpConnection::_handleError()
{
    // loop_->assertInLoopThread();
    int err = sockets::getSocketError(socket_->fd());
    LOG_ERROR << "name = " << name_
              << ", fd = " << socket_->fd()
              << ", error = " << errorString(err)
              << "(" << err << ")";
}

void tcpConnection::shutdown()
{
    loop_->runInLoop(std::bind(&tcpConnection::_shutdownInLoop, shared_from_this()));
}
void tcpConnection::_shutdownInLoop()
{
    loop_->assertInLoopThread();
    if(!channel_->isWriting()) {
        socket_->shutdownWrite();
    }
}

void tcpConnection::forceClose()
{
    if(connState_ == disconnecting || connState_ == connected) {
        setState(disconnecting);
        loop_->runInLoop(std::bind(&tcpConnection::_forceCloseInLoop, shared_from_this()));
    }
}

void tcpConnection::_forceCloseInLoop()
{
    loop_->assertInLoopThread();
    if(connState_ == connected || connState_ == disconnecting) {
        _handleClose();
    }
}

void tcpConnection::ctorConnection()
{
    loop_->assertInLoopThread();
    assert(connState_ == connecting);
    setState(connected);
    channel_->tie(shared_from_this());
    channel_->enableReading();

    connectionCallback_(shared_from_this());
}
void tcpConnection::dtorConnection()
{
    loop_->assertInLoopThread();
    if(connState_ == disconnecting) {
        setState(disconnected);
        channel_->disableAll();
        connectionCallback_(shared_from_this());
    }
    channel_->remove();
}