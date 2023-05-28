#include <learning/net/http/httpRequest.h>
#include <learning/net/buffer.h>
#include <learning/base/logging.h>

#include <regex>

using namespace learning;
using namespace learning::net;

httpRequest::httpRequest()
  : parseState_(PARSELINE),
    version_(HTTPUNKNOWN)
{}
httpRequest::~httpRequest() = default;

bool httpRequest::parse(buffer* data, timeStamp t)
{
    assert(data);
    bool ret = true;
    bool stop = false;
    while((!stop) && (parseState_ != FINISHED)) {
        const std::string line(data->getLine());
        switch(parseState_) {
            case PARSELINE:{
                bool k = _parseLine(line);
                if(!k) {
                    LOG_TRACE << "praseLine error";
                    ret = false;
                    stop = true;
                }
                setReceiveTime(t);
                parseState_ = PARSEHEADER;
                break;
            }
            case PARSEHEADER:{
                _parseHeader(line);
                if(line.size() == 0) {
                    parseState_ = PARSEBODY;
                }
                break;
            }
            case PARSEBODY:{
                _parseBody(line);
                parseState_ = FINISHED;
                stop = true;
                break;
            }
            default:{
                // 出啥事了
                ret = false;
                break;
            }
        };
    }
    return ret;
}

const std::map<std::string, VERSION> VERSIONToString{
    {"1.0", HTTP10},
    {"1.1", HTTP11}
};

bool httpRequest::_parseLine(const std::string& line)
{
    std::regex pattern("^([^ ]*) ([^ ]*) HTTP/([^ ]*)$");
    std::smatch match;
    if(std::regex_match(line, match, pattern)) {
        method_ = match[1];
        path_ = match[2];
        if(VERSIONToString.count(match[3]) == 1) {
            version_ = VERSIONToString.at(match[3]);
        }
        else {
            version_ = HTTPUNKNOWN;
            return false;
        }
        return true;
    }
    return false;
}
void httpRequest::_parseHeader(const std::string& line)
{
    std::regex pattern("^([^ ]*): ([^ ]*)$");
    std::smatch match;
    if(std::regex_match(line, match, pattern)) {
        headers_[match[1]] = match[2];
    }
}
void httpRequest::_parseBody(const std::string& line)
{
    body_ = std::move(line);
    // nothing
}