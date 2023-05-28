#ifndef LEARNING_NET_SOCKET_H
#define LEARNING_NET_SOCKET_H

#include <learning/base/noncopyable.h>
#include <learning/net/inetAddress.h>

struct sockaddr;

namespace learning {
namespace net {

class inetAddress;

class socket: noncopyable
{
public:
    socket(int fd)
      : fd_(fd) {}
    ~socket();

    int fd() const { return fd_; }
    void bind(const inetAddress& addr);
    void listen();
    int accept(inetAddress*);

    void shutdownWrite();

    void setKeepAlive(bool on);
    void setReuseAddr(bool on);
    void setReusePort(bool on);
    void setTcpNoDelay(bool on);

private:
    int fd_;
};

} // namespace net
} // namespace learning

#endif // LEARNING_NET_SOCKET_H