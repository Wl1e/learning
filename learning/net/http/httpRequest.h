#ifndef LEARNING_NET_HTTP_HTTPREQUEST_H
#define LEARNING_NET_HTTP_HTTPREQUEST_H

#include <learning/base/copyable.h>
#include <learning/base/timeStamp.h>
#include <learning/net/http/httpVersion.h>

#include <string>
#include <unordered_map>

namespace learning {
namespace net {

class buffer;

class httpRequest: public learning::copyable
{
public:
    httpRequest();
    ~httpRequest();

    enum STATE{
        PARSELINE,
        PARSEHEADER,
        PARSEBODY,
        FINISHED
    };
    bool parse(buffer* data, timeStamp t);
    VERSION version() const
    { return version_; }
    const std::string& path() const
    { return path_; }
    const std::string& method() const
    { return method_; }
    const std::string& body() const
    { return body_; }
    bool getAll() const
    { return parseState_ == FINISHED; }
    void setReceiveTime(timeStamp t)
    { receiveTime_ = t; }
    timeStamp receiveTime() const
    { return receiveTime_; }
    const std::string getHeader(const std::string& key) const
    {
        auto it = headers_.find(key);
        if(it != headers_.end()) {
            return it->second;
        }
        return std::string();
    }
    std::unordered_map<std::string, std::string> headers() const
    { return headers_; }

    void clear()
    {
        parseState_ = PARSELINE;
        method_.clear();
        path_.clear();
        body_.clear();
        headers_.clear();
        version_ = HTTPUNKNOWN;
    }

private:
    
    bool _parseLine(const std::string& line);
    void _parseHeader(const std::string& line);
    void _parseBody(const std::string& line);

private:
    STATE parseState_;

    std::string method_;
    std::string path_;
    VERSION version_;
    std::string body_;
    timeStamp receiveTime_;

    std::unordered_map<std::string, std::string> headers_;
};

} // namespace net
} // namespace learning

#endif // LEARNING_NET_HTTP_HTTPREQUEST_H