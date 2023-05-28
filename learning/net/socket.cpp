#include <learning/net/socket.h>
#include <learning/net/socketOps.h>
#include <learning/base/logging.h>


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>

using namespace learning;
using namespace learning::net;

socket::~socket()
{
    sockets::close(fd_);
}

void socket::bind(const inetAddress& addr)
{
    sockets::bind(fd_, addr.getSockaddr());
}
void socket::listen()
{
    sockets::listen(fd_);
}
int socket::accept(inetAddress* addr)
{
    sockaddr_in6 addr6;
    int ret = sockets::accept(fd_, &addr6);
    addr->setSockaddr(addr6);
    return ret;
}
void socket::shutdownWrite()
{
    sockets::shutdownWrite(fd_);
}

void socket::setKeepAlive(bool on)
{
    int opt = on ? 1 : 0;
    if(setsockopt(fd_, SOL_TCP, SO_KEEPALIVE, &opt, sizeof opt) != 0) {
        LOG_SYSERR << "setKeepAlive error";
    }
}
void socket::setReuseAddr(bool on)
{
    int opt = on ? 1 : 0;
    if(setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt) != 0) {
        LOG_SYSERR << "setReuseAddr error";
    }
}
void socket::setReusePort(bool on)
{
    int opt = on ? 1 : 0;
    if(setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof opt) != 0) {
        LOG_SYSERR << "setReusePort error";
    }
}
void socket::setTcpNoDelay(bool on)
{
    int opt = on ? 1 : 0;
    if(setsockopt(fd_, SOL_TCP, TCP_INFO, &opt, sizeof opt) != 0) {
        LOG_SYSERR << "setTcpNoDelay error";
    }
}