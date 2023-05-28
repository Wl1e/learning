#ifndef LEARNING_NET_HTTP_HTTPRESPONSE_H
#define LEARNING_NET_HTTP_HTTPRESPONSE_H

#include <learning/base/noncopyable.h>
#include <learning/base/fileUtil.h>
#include <learning/net/http/httpVersion.h>

#include <string>
#include <unordered_map>

namespace learning {

namespace fileUtil{
class readFile;
}

namespace net {

class buffer;

class httpResponse: noncopyable
{
public:
    httpResponse(const std::string& path);
    ~httpResponse();

    enum CODE{
        unknown = 0,
        OK = 200,
        MovedPermanently = 301,
        BadRequest = 400,
        Forbidden = 403,
        NotFound = 404
    };

    void process(buffer* buffer);
    void setHeader(const std::string& key, const std::string& value)
    { headers_[key] = value; }
    void setHeaders(std::unordered_map<std::string, std::string> headers)
    { headers_ = std::move(headers); }
    size_t delHeader(const std::string& key)
    {
        return headers_.erase(key);
    }
    void setVersion(VERSION version)
    { version_ = version; }
    void setKeepAlive(bool on)
    {
        headers_["Connection"] = on ? "Keep-Alive" : "close";
    }
    bool isCloseConn() const
    { return (headers_.count("Connection") == 0) || (headers_.at("Connection") == "close"); }
    void setBody(const std::string& data)
    { body_ = data; }

private:
    void _addLine();
    void _addHeader();
    void _addBody();
    void _addLRCF();
    void _setBodyFromFile(fileUtil::readFile* prf);

    void _errorHtml();
    void _setCode(int res);

private:
    VERSION version_;
    CODE code_;
    std::string path_;
    std::string body_;
    std::unordered_map<std::string, std::string> headers_;

    buffer* outputBuf_;
};

} // namespace net
} // namespace learning

#endif // LEARNING_NET_HTTP_HTTPRESPONSE_H