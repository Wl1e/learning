#include <learning/base/logging.h>
#include <learning/base/currentThread.h>

using namespace learning;

namespace learning {

const char* level[]{
    "TRACE",
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "FATAL"
};

std::string serverName = "server";
// void setServerName(const std::string& name)
// {
//   serverName = name;
// }

static char buf[64];
std::string errorString(int error)
{
    return strerror_r(error, buf, sizeof buf);
}

void defaultOutputFunction(const char* data, int len)
{
    fwrite(data, 1, len, stdout);
}
void defaultFlushFunction()
{
    fflush(stdout);
}
logging::LOG_LEVEL initLogLevel()
{
    if(::getenv("USE_LOG_TRACE")) {
        return logging::TRACE;
    }
    else if(::getenv("USE_LOG_DEBUG")) {
        return logging::DEBUG;
    }
    else {
        return logging::INFO;
    }
}

logging::LOG_LEVEL globalLevel = initLogLevel();
logging::outputFunction outputF = defaultOutputFunction;
logging::flushFunction flushF = defaultFlushFunction;

} // namespace learning

void logging::setLogLevel(LOG_LEVEL level)
{ globalLevel = level; }
void logging::setOutputFunction(outputFunction f)
{ outputF = f; }
void logging::setFlushFunction(flushFunction f)
{ flushF = f; }



logging::logging(sourceFile file, int line)
  : imp1_(file, line, LOG_LEVEL::INFO, 0) {}
logging::logging(sourceFile file, int line, LOG_LEVEL level)
  : imp1_(file, line, level, 0) {}
logging::logging(sourceFile file, int line, LOG_LEVEL level, const char* f)
  : imp1_(file, line, level, 0)
{
    imp1_.stream_ << f << ' ';
}
logging::logging(sourceFile file, int line, bool isAbort)
  : imp1_(file, line, isAbort?FATAL:ERROR, errno)
{}
logging::~logging()
{
    imp1_.finish();
    const logStream::buffer& buf(imp1_.stream_.getBuffer());
    // logFile file(serverName, 3000);
    // using func = void(logFile::*)(const char*, size_t);
    // setOutputFunction(std::bind(static_cast<func>(&logFile::append), &file, std::placeholders::_1, std::placeholders::_2));
    // setFlushFunction(std::bind(&logFile::flush, &file));
    outputF(buf.getData(), buf.size());
    if(globalLevel == FATAL) {
        flushF();
        abort();
    }
}

logging::imp1::imp1(const sourceFile& file, int line, LOG_LEVEL logLevel, int error)
  : file_(file.file, file.size),
    line_(line),
    logLevel_(logLevel)
{
    currentThread::tid();
    stream_ << currentThread::tidToString() << ' ';
    stream_ << level[logLevel] << ' ';
    // stream_ << ' ' << serverName << ": ";
    if(error != 0) {
        stream_ << errorString(error) << "(error is " << error << ")";
    }
}

logging::imp1::~imp1()
{
    finish();
}
void logging::imp1::finish()
{
    stream_ << ' ' << file_ << " - " << line_ << '\n';
}