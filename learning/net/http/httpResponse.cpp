#include <learning/net/http/httpResponse.h>
#include <learning/net/buffer.h>
#include <learning/base/fileUtil.h>
#include <learning/base/logging.h>

#include <sys/stat.h>
#include <assert.h>

using namespace learning;
using namespace learning::net;

const std::string defaultMethod{"POST"};
std::unordered_map<int, const char*> code{
    {0, "unknown"},
    {200, "ok"},
    {301, "moved Permanently"},
    {400, "bad Request"},
    {403, "Forbidden"},
    {404, "not Found"}
};

httpResponse::httpResponse(const std::string& path)
  : version_(HTTPUNKNOWN),
    code_(unknown),
    path_(path),
    outputBuf_(nullptr)
{}
httpResponse::~httpResponse() = default;

void httpResponse::process(buffer* buffer)
{
    assert(buffer);
    outputBuf_ = buffer;
    fileUtil::readFile rf(path_);
    int res = rf.opened();
    _setCode(res);
    LOG_TRACE << "code = " << code_;
    _setBodyFromFile(&rf);
    _addLine();
    _addHeader();
    _addBody();
}

void httpResponse::_setBodyFromFile(fileUtil::readFile* prf)
{
    /*  不为0就代表调用过setBody, 就不设置了 */
    if(body_.size() != 0) {
        return;
    }
    if(code_ != OK) {
        _errorHtml();
    }
    else {
        int error;
        /*  有点冗余了, 这里只是为了获取文件大小方便提取
            readFile判断了errno, 这里也进行了判断, 能不能省省?
        */
        struct stat fileStat;
        stat(path_.data(), &fileStat);
        setBody(prf->readToString(fileStat.st_size, &error));
    }
}

void httpResponse::_addLine()
{
    char buf[32];
    snprintf(buf, sizeof buf, "HTTP/%.1f %d %s", version_/10.0, code_, code[code_]);
    outputBuf_->append(buf);
    _addLRCF();
}
void httpResponse::_addHeader()
{
    for(const auto& it : headers_) {
        outputBuf_->append(it.first + ": " + it.second + "\r\n");
    }
    outputBuf_->append("Content-Length: " + std::to_string(body_.size()) + "\r\n");
    _addLRCF();
}
void httpResponse::_addBody()
{
    outputBuf_->append(body_);
    _addLRCF();
}
void httpResponse::_addLRCF()
{ outputBuf_->append("\r\n"); }
void httpResponse::_errorHtml()
{
    body_ = R"(<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
</head>
<body>
    <h1>)";

    body_ += std::to_string(code_) + ' ' + code[code_] + "\r\n";
    body_ += R"(</h1>
</body>
</html>)";
}

/*
EPERM 1     不允许操作
ENOENT 2    没有这样的文件或目录
ESRCH 3     没有这样的过程
EINTR 4     系统调用中断
EIO 5       I/O 错误
ENXIO 6     没有这样的设备或地址
E2BIG 7     参数列表太长
ENOEXEC 8   执行格式错误
EBADF 9     文件编号错误
ECHILD 10   无子进程
EAGAIN 11   再试一次
ENOMEM 12   内存不足
EACCES 13   权限被拒绝
EFAULT 14   地址错误
ENOTBLK 15  需要块设备
EBUSY 16    设备或资源繁忙
EEXIST 17   文件存在
EXDEV 18    跨设备链接
ENODEV 19   没有这样的设备
ENOTDIR 20  不是目录
EISDIR 21   是一个目录
EINVAL 22   参数无效
ENFILE 23   文件表溢出
EMFILE 24   打开的文件太多
ENOTTY 25   不是打字机
ETXTBSY 26  文本文件繁忙
EFBIG 27    文件太大
ENOSPC 28   设备上没有剩余空间
ESPIPE 29   非法寻求
EROFS 30    只读文件系统
EMLINK 31   链接太多
EPIPE 32    破管
EDOM 33     函数域之外的数学论证
ERANGE 34   数学结果无法表示
*/

void httpResponse::_setCode(int res)
{
    switch(res) {
        case EEXIST:
        case 0: {
            code_ = OK;
            break;
        }
        case EISDIR:
        case ENOENT: {
            code_ = NotFound;
            break;
        }
        case EACCES: {
            code_ = Forbidden;
            break;
        }
        default: {
            code_ = unknown;
            break;
        }
    }
}