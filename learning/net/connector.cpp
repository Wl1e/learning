#include <learning/net/connector.h>
#include <learning/net/eventLoop.h>
#include <learning/net/channel.h>
#include <learning/net/socket.h>
#include <learning/net/socketOps.h>
#include <learning/base/logging.h>

using namespace learning;
using namespace learning::net;

const int defaultRetryIntervalMS = 500;
const int maxRetryIntervalMS = 30 * 1000;

connector::connector(eventLoop* loop, const inetAddress& addr)
  : loop_(loop),
    addr_(addr),
    started_(false),
    state_(disconnected),
    retryIntervalMS_(0)
{
    LOG_DEBUG << "[" << this << "]ctor";
}
connector::~connector()
{
    LOG_DEBUG << "[" << this << "]dtor";
    assert(!channel_);
}

void connector::start()
{
    loop_->runInLoop(std::bind(&connector::_runInLoop, this));
    started_ = true;
}
void connector::restart()
{
    loop_->assertInLoopThread();
    _setState(disconnected);
    retryIntervalMS_ = defaultRetryIntervalMS;
    started_ = true;
    _runInLoop();
}
void connector::stop()
{
    loop_->runInLoop(std::bind(&connector::_stopInLoop, this));
    started_ = false;
}

void connector::_runInLoop()
{
    loop_->assertInLoopThread();
    assert(state_ == disconnected);
    if(started_) {
        _connect();
    }
    else {
        LOG_DEBUG << "not connect";
    }
}
void connector::_stopInLoop()
{
    loop_->assertInLoopThread();
    /*  处于connecting状态也就是还没连上, 所以删掉重连
        那处于connected就没反应吗? */
    if(state_ == connecting) {
        state_ = disconnected;
        int fd = _removeAndResetChannel();
        _retry(fd);
    }
}
void connector::_connect()
{
    int fd = sockets::createNonblockCloseExecFd(addr_.family());
    if(fd < 0) {
        LOG_SYSERR << "create fd error";
        return;
    }
    int ret = sockets::connect(fd, addr_.getSockaddr());
    if(ret != 0) {
        int error = errno;
        switch(error) {
            case EISCONN: {
                _connecting(fd);
                break;
            }
            case ENOTSOCK: {
                sockets::close(fd);
                LOG_SYSERR << "not sock";
                break;
            }
            default: {
                sockets::close(fd);
                LOG_SYSERR << "unknown error";
                break;
            }
        }
    }
    else {
        _connecting(fd);
    }
    
}
void connector::_connecting(int fd)
{
    assert(state_ == disconnected);
    assert(!channel_);
    channel_ = std::make_unique<channel>(loop_, fd);
    channel_->setWriteCallback(std::bind(&connector::_handleWrite, this));
    channel_->setErrorCallback(std::bind(&connector::_handleError, this));
    channel_->enableWriting();
}

void connector::_handleWrite()
{
    loop_->assertInLoopThread();
    if(state_ == connecting) {
        int fd = _removeAndResetChannel();
        int error = sockets::getSocketError(fd);
        if(error) {
            LOG_WARN << "socketError, error = " << errorString(error)
                      << "(" << error << ")";
            _retry(fd);
        }
        else if(sockets::isSelfConn(fd)) {
            LOG_WARN << "selfConn";
        }
        else {
            _setState(connected);
            if(started_) {
                newConnectionCallback_(fd);
            }
            else {
                sockets::close(fd);
            }
        }
    }
    else {
        assert(state_ == disconnected);
    }
}
void connector::_handleError()
{
    LOG_ERROR << "state = " << state_;
    if(state_ == connecting) {
        int fd = _removeAndResetChannel();
        int error = sockets::getSocketError(fd);
        LOG_TRACE << "error = " << errorString(error) << "(" << error << ")";
    }
}

int connector::_removeAndResetChannel()
{
    channel_->disableAll();
    channel_->remove();
    int fd = channel_->fd();
    loop_->runInLoop(std::bind(&connector::_resetChannel, this));
    return fd;
}
void connector::_resetChannel()
{
    channel_.reset();
}

void connector::_retry(int fd)
{
    sockets::close(fd);
    _setState(disconnected);
    if(started_) {
        LOG_TRACE << "a connection retry";
        loop_->runAfter(std::bind(&connector::_runInLoop, this), retryIntervalMS_/1000.0);
    }
}