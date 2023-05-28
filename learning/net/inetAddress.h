#ifndef LEARNING_NET_INETADDRESS_H
#define LEARNING_NET_INETADDRESS_H

#include <learning/base/copyable.h>
#include <learning/net/socketOps.h>

#include <string>
#include <arpa/inet.h>

namespace learning {
namespace net {

class inetAddress: public learning::copyable
{
public:
    inetAddress(sa_family_t fam, const char* ip, in_port_t port);
    inetAddress(in_port_t port = 0, bool loopBack = false, bool isIP6 = false);
    inetAddress(const sockaddr_in& addr4)
      : addr4_(addr4) {}
    inetAddress(const sockaddr_in6& addr6)
      : addr6_(addr6) {}
    ~inetAddress();

    sa_family_t family() const;
    std::string ip() const;
    in_port_t port() const;
    std::string ipPort() const;

    void setSockaddr(const sockaddr_in6& addr)
    { addr6_ = addr; }
    const sockaddr* getSockaddr() const
    { return sockets::toSockaddr(&addr6_); }

private:
    union{
        sockaddr_in addr4_;
        sockaddr_in6 addr6_;
    };
};

} // namespace net
} //namespace learning

#endif // LEARNING_NET_INETADDRESS_H