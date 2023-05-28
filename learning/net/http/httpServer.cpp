#include <learning/net/http/httpServer.h>
#include <learning/net/eventLoop.h>
#include <learning/net/http/httpRequest.h>
#include <learning/net/http/httpResponse.h>
#include <learning/base/logging.h>

using namespace learning;
using namespace learning::net;

namespace learning::net::detail{
void defaultHttpCallback(const httpRequest& req, httpResponse& res)
{
    res.setVersion(req.version());
    // res.setHeaders(req.headers());
    std::string conn = req.getHeader("Connection");
    if(conn.empty()) {
        conn = "close";
    }
    res.setHeader("Connection", conn);
}
} // namespace learning::net::detail

httpServer::httpServer(eventLoop* loop,
                       const std::string& name,
                       const inetAddress& addr,
                       bool reusePort)
  : server_(loop, name, addr, reusePort),
    callback_(detail::defaultHttpCallback)
{
    server_.setConnectionCallback(std::bind(&httpServer::_handleConnection, this, _1));
    server_.setMessageCallback(std::bind(&httpServer::_handleMessage, this, _1, _2, _3));
}

void httpServer::start()
{
    LOG_DEBUG << "httpServer[" << name() << "] start";
    server_.start();
}

void httpServer::_handleConnection(const tcpConnectionPtr& conn)
{
    if(conn->isConnected()) {
        conn->setContext(httpRequest());
    }
}
void httpServer::_handleMessage(const tcpConnectionPtr& conn, buffer* buf, timeStamp t)
{
    httpRequest* req = std::any_cast<httpRequest>(conn->contextPtr());
    if(!req->parse(buf, t)) {
        conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
        conn->shutdown();
    }
    if(req->getAll()) {
        _handleRequest(conn, *req);
        req->clear();
    }
}
void httpServer::_handleRequest(const tcpConnectionPtr& conn, const httpRequest& req)
{
    LOG_TRACE << "path = " << req.path();
    httpResponse res(req.path());
    callback_(req, res);
    buffer buf;
    res.process(&buf);
    conn->send(&buf);
    if(res.isCloseConn()) {
        conn->shutdown();
    }
}