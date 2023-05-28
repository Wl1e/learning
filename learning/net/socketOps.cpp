#include <learning/base/logging.h>
#include <learning/net/socketOps.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/uio.h>

using namespace learning;

int sockets::createNonblockCloseExecFd(sa_family_t family)
{
    int fd = ::socket(family, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    if(fd < 0) {
        LOG_SYSFATAL << "create fd error";
    }
    return fd;
}
void sockets::setFdNonblockCloseExec(int fd)
{
    /*  设置非阻塞 */
    int flags = fcntl(fd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    fcntl(fd, F_SETFL, flags);
    /*  设置closeExec */
    flags = fcntl(fd, F_GETFL, 0);
    flags |= O_CLOEXEC;
    fcntl(fd, F_SETFL, flags);
}
void sockets::bind(int fd, const sockaddr* addr)
{
    uint32_t length = (addr->sa_family == AF_INET6 ? sizeof(sockaddr_in6) : sizeof(sockaddr_in));
    if(::bind(fd, addr, static_cast<socklen_t>(length)) != 0) {
        LOG_SYSFATAL << "bind error";
    }
}
void sockets::listen(int fd)
{
    if(::listen(fd, SOMAXCONN) != 0) {
        LOG_SYSFATAL << "listen error";
    }
}
int sockets::accept(int fd, sockaddr_in6* addr6)
{
    socklen_t len = sizeof addr6;
#if VALGRIND || defined(NO_ACCEPT4)
    int fd2 = ::accept(fd, toSockaddr(&addr6), &len);
    setFdNonblockCloseExec(fd);
#else
    int fd2 = ::accept4(fd, toSockaddr(addr6), &len, O_NONBLOCK | O_CLOEXEC);
#endif
    if(fd2 < 0) {
        int savedErrno = errno;
        switch (errno)
        {
        case EMFILE:
            errno = savedErrno;
            break;
        case EOPNOTSUPP:
            LOG_FATAL << "unexpectd error";
            break;
        default:
            LOG_FATAL << "unknown error";
            break;
        }
    }
    return fd2;
}

int sockets::connect(int fd, const sockaddr* addr)
{
    return connect(fd, addr, sizeof(sockaddr_in6));
}

ssize_t sockets::read(int fd, void* data, size_t len)
{
    return ::read(fd, data, len);
}
ssize_t sockets::readv(int fd, iovec* iov, int iovCnt)
{
    return ::readv(fd, iov, iovCnt);
}
ssize_t sockets::write(int fd, const void* data, size_t len)
{
    return ::write(fd, data, len);
}
void sockets::close(int fd)
{
    if(::close(fd) != 0) {
        LOG_SYSFATAL << "close fd error, fd = " << fd;
    }
}
void sockets::shutdownWrite(int fd)
{
    if(::shutdown(fd, SHUT_WR) != 0) {
        LOG_SYSFATAL << "shutdownWrite error, fd = " << fd;
    }
}
int sockets::getSocketError(int fd)
{
    int error;
    socklen_t len = static_cast<socklen_t>(sizeof error);
    if(::getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &len) == 0) {
        return error;
    }
    else {
        return -1;
    }
}

sockaddr_in6 sockets::getLocalAddr(int fd)
{
    sockaddr_in6 addr6;
    socklen_t len = sizeof addr6;
    if(getsockname(fd, toSockaddr(&addr6), &len) != 0) {
        LOG_SYSFATAL << "getsockname error, fd = " << fd;
    }
    return addr6;
}
sockaddr_in6 sockets::getPeerAddr(int fd)
{
    sockaddr_in6 addr6;
    socklen_t len = sizeof addr6;
    if(getpeername(fd, toSockaddr(&addr6), &len) != 0) {
        LOG_SYSFATAL << "getpeername error, fd = " << fd;
    }
    return addr6;
}
bool sockets::isSelfConn(int fd)
{
    sockaddr_in6 localAddr = getLocalAddr(fd);
    sockaddr_in6 peerAddr = getPeerAddr(fd);
    if(localAddr.sin6_family == AF_INET6) {
        return (localAddr.sin6_family == peerAddr.sin6_family) && (localAddr.sin6_port == peerAddr.sin6_port) && memcmp(&(localAddr.sin6_addr), &(peerAddr.sin6_addr), sizeof(peerAddr.sin6_addr) == 0);
    }
    else if(localAddr.sin6_family == AF_INET) {
        const sockaddr_in* lhs = toSockaddr4(toSockaddr(&localAddr));
        const sockaddr_in* rhs = toSockaddr4(toSockaddr(&peerAddr));
        return (lhs->sin_addr.s_addr == rhs->sin_addr.s_addr) && (lhs->sin_family == rhs->sin_family) && (lhs->sin_port == rhs->sin_port);
    }
    else {
        return false;
    }
}

const sockaddr* sockets::toSockaddr(const sockaddr_in* addr4)
{
    return reinterpret_cast<const sockaddr*>(addr4);
}
const sockaddr* sockets::toSockaddr(const sockaddr_in6* addr6)
{
    return reinterpret_cast<const sockaddr*>(addr6);
}
sockaddr* sockets::toSockaddr(sockaddr_in6* addr6)
{
    return reinterpret_cast<sockaddr*>(addr6);
}
const sockaddr_in* sockets::toSockaddr4(const sockaddr* addr)
{
    return reinterpret_cast<const sockaddr_in*>(addr);
}
const sockaddr_in6* sockets::toSockaddr6(const sockaddr* addr)
{
    return reinterpret_cast<const sockaddr_in6*>(addr);
}

std::string sockets::ip(const sockaddr* addr)
{
    char buf[32];
    if(addr->sa_family == AF_INET6) {
        const sockaddr_in6* addr6 = toSockaddr6(addr);
        inet_ntop(addr6->sin6_family, &(addr6->sin6_addr), buf, INET6_ADDRSTRLEN);
    }
    else {
        const sockaddr_in* addr4 = toSockaddr4(addr);
        inet_ntop(addr4->sin_family, &(addr4->sin_addr), buf, INET_ADDRSTRLEN);
    }
    return buf;
}

std::string sockets::ipPort(const sockaddr* addr)
{
    std::string ret = ip(addr);
    if(addr->sa_family == AF_INET6) {
        ret += "[" + std::to_string(be16toh(toSockaddr6(addr)->sin6_port)) + "]";
    }
    else {
        ret += ":" + std::to_string(be16toh(toSockaddr4(addr)->sin_port));
    }
    return ret;
}

void sockets::fromIpPort(sockaddr_in* addr4, const char* ip, in_port_t port)
{
    addr4->sin_family = AF_INET;
    addr4->sin_port = htobe16(port);
    int res = inet_pton(addr4->sin_family, ip, &addr4->sin_addr.s_addr);
    if(res != 1) {
        if(res == 0) {
            LOG_ERROR << "ip is error";
        }
        else if(res < 0) {
            LOG_ERROR << "unknown error";
        }
    }
}
void sockets::fromIpPort(sockaddr_in6* addr6, const char* ip, in_port_t port)
{
    addr6->sin6_family = AF_INET6;
    addr6->sin6_port = htobe16(port);
    int res = inet_pton(addr6->sin6_family, ip, &addr6->sin6_addr);
    if(res != 1) {
        if(res == 0) {
            LOG_ERROR << "ip is error";
        }
        else if(res < 0) {
            LOG_ERROR << "unknown error";
        }
    }
}