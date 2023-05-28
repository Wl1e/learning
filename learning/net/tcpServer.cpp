#include <learning/net/tcpServer.h>
#include <learning/net/acceptor.h>
#include <learning/net/eventLoop.h>
#include <learning/net/eventLoopThreadPool.h>
#include <learning/net/socketOps.h>
#include <learning/base/logging.h>

using namespace learning;
using namespace learning::net;

const inetAddress localAddr = INADDR_ANY;


tcpServer::tcpServer(eventLoop* loop,
                     const std::string& name,
                     const inetAddress& addr,
                     bool reusePort)
  : name_(name),
    loop_(loop),
    started_(false),
    acceptor_(new acceptor(loop_, addr, reusePort)),
    connCnt_(0),
    loopPool_(new eventLoopThreadPool(loop_)),
    connectionCallback_(defaultConnectionCallback),
    messageCallback_(defaultMessageCallback)
{
    acceptor_->setNewConnectionCallback(
        std::bind(&tcpServer::_newConn, this, _1, _2));
    // setServerName(name);
}
tcpServer::~tcpServer()
{
    loop_->assertInLoopThread();
    LOG_TRACE << "server[" << name_ << "] dtor";
    started_ = false;
    for(auto& it : conns_) {
        tcpConnectionPtr p(it.second);
        it.second.reset();
        loopPool_->getNextLoop()->runInLoop(std::bind(&tcpConnection::dtorConnection, p));
    }
}

void tcpServer::setThreadCnt(size_t n)
{
    loopPool_->setThreadNums(n);
}

void tcpServer::start()
{
    if(started_ == false) {
        started_ = true;
        loopPool_->start(threadInitCallback_);
        assert(!acceptor_->isListening());
        loop_->runInLoop(std::bind(&acceptor::listen, acceptor_.get()));
    }
}

void tcpServer::_newConn(int fd, const inetAddress& addr)
{
    loop_->assertInLoopThread();
    ++connCnt_;
    LOG_INFO << "a new connection, fd = " << fd
                << ", from " << addr.ipPort();
    eventLoop* loop = loopPool_->getNextLoop();
    inetAddress LocalAddr(sockets::getLocalAddr(fd));
    tcpConnectionPtr newConn = std::make_shared<tcpConnection>("connection" + std::to_string(connCnt_),
                                                               loop,
                                                               fd,
                                                               LocalAddr,
                                                               addr);
    conns_[newConn->name()] = newConn;
    newConn->setConnectionCallback(connectionCallback_);
    newConn->setMessageCallback(messageCallback_);
    newConn->setWriteCompleteCallback(writeCompleteCallback_);
    newConn->setCloseCallback(std::bind(&tcpServer::_removeConn, this, _1));

    loop->runInLoop(std::bind(&tcpConnection::ctorConnection, newConn));
}

void tcpServer::_removeConn(const tcpConnectionPtr& conn)
{
    loop_->runInLoop(std::bind(&tcpServer::_removeConnInLoop, this, conn));
}

void tcpServer::_removeConnInLoop(const tcpConnectionPtr& conn)
{
    loop_->assertInLoopThread();
    std::string name(conn->name());
    size_t ret = conns_.erase(name); (void)ret;
    if(ret == 1) {
        LOG_INFO << "connection[" << name << "] "
                << "close connection";
        conns_[name].reset();
        loopPool_->getNextLoop()->runInLoop(std::bind(&tcpConnection::dtorConnection, conn));
    }
}