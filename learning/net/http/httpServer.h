#ifndef LEARNING_NET_HTTP_HTTPSERVER_H
#define LEARNING_NET_HTTP_HTTPSERVER_H

#include <learning/base/noncopyable.h>
#include <learning/net/tcpServer.h>
#include <learning/net/callback.h>
#include <learning/base/timeStamp.h>

#include <functional>

namespace learning {
namespace net {

class buffer;
class eventLoop;
class httpRequest;
class httpResponse;

class httpServer: noncopyable
{
public:
    httpServer(eventLoop* loop,
               const std::string& name,
               const inetAddress& addr,
               bool reusePort = false);
    ~httpServer() = default;

    void start();
    using httpCallback = std::function<void(const httpRequest& req, httpResponse& res)>;
    void setReqResCallback(const httpCallback& cb)
    { callback_ = cb; }
    const std::string& name() const
    { return server_.name(); }
    void setThreadNums(size_t nums)
    { server_.setThreadCnt(nums); }
    eventLoop* loop() const
    { return server_.loop(); }

private:
    void _handleConnection(const tcpConnectionPtr& conn);
    void _handleMessage(const tcpConnectionPtr& conn, buffer* buf, timeStamp t);
    void _handleRequest(const tcpConnectionPtr& conn, const httpRequest&);

private:
    tcpServer server_;
    httpCallback callback_;
};

} // namespace net
} // namespace learning

#endif // LEARNING_NET_HTTP_HTTPSERVER_H