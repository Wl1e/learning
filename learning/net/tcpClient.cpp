#include <learning/net/tcpClient.h>
#include <learning/net/tcpConnection.h>
#include <learning/net/eventLoop.h>
#include <learning/net/connector.h>
#include <learning/base/logging.h>

#include <assert.h>

using namespace learning;
using namespace learning::net;

namespace learning::net::detail{

void removeConn(eventLoop* loop, const tcpConnectionPtr& conn)
{
    loop->runInLoop(std::bind(&tcpConnection::dtorConnection, conn));
}
void removeConnector(const connectorPtr& conn)
{
    conn->stop();
}

} // namespace learning::net::detail

tcpClient::tcpClient(eventLoop* loop, const std::string& name, const inetAddress& addr)
  : loop_(loop),
    name_(name),
    connected_(false),
    connector_(new connector(loop_, addr)),
    connCnt_(0),
    connectionCallback_(defaultConnectionCallback),
    messageCallback_(defaultMessageCallback)
{
    connector_->setNewConnectionCallback(std::bind(&tcpClient::_newConnection, this, _1));
    LOG_INFO << "tcpClient[" << name_ << "] created";
}
tcpClient::~tcpClient()
{
    tcpConnectionPtr conn;
    bool unique = false;
    {
        std::lock_guard<std::mutex> guard(mutex_);
        unique = conn_.unique();
        conn = conn_;
    }
    if (conn)
    {
        assert(loop_ == conn->loop());
        // FIXME: not 100% safe, if we are in different thread
        closeCallback cb = std::bind(&detail::removeConn, loop_, _1);
        loop_->runInLoop(
            std::bind(&tcpConnection::setCloseCallback, conn, cb));
        if (unique)
        {
        conn->forceClose();
        }
    }
    else
    {
        connector_->stop();
        // FIXME: HACK
        loop_->runAfter(std::bind(&detail::removeConnector, connector_), 1);
    }
}

void tcpClient::connect()
{
    connected_ = true;
    LOG_INFO << "tcpClient[" << name_ << "] connect to " << connector_->addr().ipPort();
    connector_->start();
}
void tcpClient::disconnect()
{
    connected_ = false;
    std::lock_guard<std::mutex> guard(mutex_);
    if(conn_) {
        conn_->shutdown();
    }
}
void tcpClient::stop()
{
    connected_ = false;
    connector_->stop();
}

void tcpClient::_newConnection(int fd)
{
    loop_->assertInLoopThread();
    inetAddress peerAddr(sockets::getPeerAddr(fd));
    ++connCnt_;
    tcpConnectionPtr newConn(new tcpConnection(
                             "connection" + peerAddr.ipPort() + '#' + std::to_string(connCnt_),
                             loop_,
                             fd,
                             sockets::getLocalAddr(fd),
                             peerAddr
    ));
    newConn->setConnectionCallback(connectionCallback_);
    newConn->setMessageCallback(messageCallback_);
    newConn->setWriteCompleteCallback(writeCompleteCallback_);
    newConn->setCloseCallback(std::bind(&tcpClient::_removeConnection, this, _1));
    {
        std::lock_guard<std::mutex> guard(mutex_);
        conn_ = newConn;
    }
    conn_->ctorConnection();
}
void tcpClient::_removeConnection(const tcpConnectionPtr& conn)
{
    loop_->assertInLoopThread();
    assert(loop_ == conn->loop());
    {
        std::lock_guard<std::mutex> guard(mutex_);
        assert(conn == conn_);
        conn_.reset();
    }
    loop_->runInLoop(std::bind(&tcpConnection::dtorConnection, conn));
}