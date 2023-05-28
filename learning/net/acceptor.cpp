#include <learning/net/acceptor.h>
#include <learning/net/channel.h>
#include <learning/net/socketOps.h>
#include <learning/net/eventLoop.h>
#include <learning/base/logging.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>

using namespace learning;
using namespace learning::net;

acceptor::acceptor(eventLoop* loop, const inetAddress& addr, bool reusePort)
  : loop_(loop),
    fd_(sockets::createNonblockCloseExecFd(addr.family())),
    channel_(loop_, fd_.fd()),
    listening_(false),
    remainFd_(open("/dev/null", O_RDONLY | O_CLOEXEC))
{
    assert(remainFd_ >= 0);
    fd_.setReuseAddr(true);
    fd_.setReusePort(reusePort);
    fd_.bind(addr);
    channel_.setMessageCallback(std::bind(&acceptor::_handleRead, this));
    LOG_DEBUG << "acceptor bind, addr = " << addr.ipPort();
}
acceptor::~acceptor()
{
    channel_.disableAll();
    channel_.remove();
    sockets::close(remainFd_);
}

void acceptor::listen()
{
    loop_->assertInLoopThread();
    listening_ = true;
    fd_.listen();
    channel_.enableReading();
    LOG_DEBUG << "acceptor listen";
}

void acceptor::_handleRead()
{
    loop_->assertInLoopThread();
    inetAddress addr;
    int fd = fd_.accept(&addr);
    if(fd >= 0) {
        if(newConnectionCallback_) {
            newConnectionCallback_(fd, addr);
        }
        else {
            sockets::close(fd);
        }
    }
    else {
        LOG_ERROR << "accept error";
        if(errno == EMFILE) {
            sockets::close(remainFd_);
            remainFd_ = accept(fd_.fd(), nullptr, nullptr);
            sockets::close(remainFd_);
            remainFd_ = open("/dev/null", O_RDONLY | O_CLOEXEC);
        }
    }
}