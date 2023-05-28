#ifndef LEARNING_NET_SOCKETOPS_H
#define LEARNING_NET_SOCKETOPS_H

#include <arpa/inet.h>
#include <string>

namespace learning{
namespace sockets{

int createNonblockCloseExecFd(sa_family_t family);
void setFdNonblockCloseExec(int fd);
void bind(int fd, const sockaddr* addr);
void listen(int fd);
int accept(int fd, sockaddr_in6* addr6);
int connect(int fd, const sockaddr* addr);

ssize_t read(int fd, void* data, size_t len);
ssize_t readv(int fd, iovec* iov, int iovCnt);
ssize_t write(int fd, const void* data, size_t len);
void close(int fd);
void shutdownWrite(int fd);
int getSocketError(int fd);

sockaddr_in6 getLocalAddr(int fd);
sockaddr_in6 getPeerAddr(int fd);
bool isSelfConn(int fd);

const sockaddr* toSockaddr(const sockaddr_in* addr4);
const sockaddr* toSockaddr(const sockaddr_in6* addr6);
sockaddr* toSockaddr(sockaddr_in6* addr6);
const sockaddr_in* toSockaddr4(const sockaddr* addr);
const sockaddr_in6* toSockaddr6(const sockaddr* addr);

std::string ip(const sockaddr* addr);
std::string ipPort(const sockaddr* addr);
void fromIpPort(sockaddr_in* addr4, const char* ip, in_port_t port);
void fromIpPort(sockaddr_in6* addr6, const char* ip, in_port_t port);

} // namespace sockets
} // namespace learning

#endif // LEARNING_NET_SOCKETOPS_H