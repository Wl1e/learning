#include <learning/net/inetAddress.h>
#include <learning/net/socketOps.h>

using namespace learning;
using namespace learning::net;

inetAddress::inetAddress(sa_family_t fam, const char* ip, in_port_t port)
{
    if(fam == AF_INET6) {
        sockets::fromIpPort(&addr6_, ip, port);
    }
    else {
        sockets::fromIpPort(&addr4_, ip, port);
    }
}
inetAddress::inetAddress(in_port_t port, bool loopBack, bool isIP6)
{
    if(isIP6) {
        addr6_.sin6_family = AF_INET6;
        addr6_.sin6_port = htobe16(port);
        addr6_.sin6_addr = loopBack ? in6addr_loopback : in6addr_any;
    }
    else {
        addr4_.sin_family = AF_INET;
        addr4_.sin_port = htobe16(port);
        addr4_.sin_addr.s_addr = loopBack ? INADDR_LOOPBACK : INADDR_ANY;
    }
}

inetAddress::~inetAddress() = default;

sa_family_t inetAddress::family() const
{ return addr4_.sin_family; }
std::string inetAddress::ip() const
{
    return sockets::ip(sockets::toSockaddr(&addr6_));
}
in_port_t inetAddress::port() const
{
    return addr6_.sin6_port;
}
std::string inetAddress::ipPort() const
{
    return sockets::ipPort(sockets::toSockaddr(&addr6_));
}