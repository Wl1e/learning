#ifndef LEARNING_BASE_LOGGING_H
#define LEARNING_BASE_LOGGING_H

#include <learning/base/noncopyable.h>
#include <learning/base/logStream.h>
#include <learning/base/logFile.h>

#include <string>
#include <string.h>
#include <functional>

namespace learning {

// void setServerName(const std::string& name);
std::string errorString(int error);

class sourceFile
{
public:
    sourceFile(const char* file1)
      : file(file1)
    {
        const char* it = strrchr(file1, '/');
        if(it) {
            file = it + 1;
        }
        size = strlen(file);
    }

    const char* file;
    size_t size;
};

class logging: noncopyable
{
public:
    enum LOG_LEVEL{
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL
    };

    logging(sourceFile file, int line);
    logging(sourceFile file, int line, LOG_LEVEL level);
    logging(sourceFile file, int line, LOG_LEVEL level, const char* f);
    logging(sourceFile file, int line, bool isAbort);
    ~logging();

    using outputFunction = std::function<void(const char*, size_t)>;
    using flushFunction = std::function<void()>;
    static void setOutputFunction(outputFunction f);
    static void setFlushFunction(flushFunction f);
    static void setLogLevel(LOG_LEVEL level);
    static LOG_LEVEL getLevel();
    logStream& stream()
    { return imp1_.stream_; }

private:
    class imp1
    {
    public:
        using LOG_LEVEL = logging::LOG_LEVEL;
        imp1(const sourceFile& fileName, int line, LOG_LEVEL logLevel, int error);
        ~imp1();

        void finish();

        std::string file_;
        int line_;
        LOG_LEVEL logLevel_;
        logStream stream_;
    };
    imp1 imp1_;
};

extern logging::LOG_LEVEL globalLevel;

inline logging::LOG_LEVEL logging::getLevel()
{ return globalLevel; }

#define LOG_TRACE \
if(learning::logging::getLevel() <= learning::logging::TRACE) \
    learning::logging(__FILE__, __LINE__, learning::logging::TRACE, __func__).stream()
#define LOG_DEBUG \
if(learning::logging::getLevel() <= learning::logging::DEBUG) \
    learning::logging(__FILE__, __LINE__, logging::DEBUG, __func__).stream()
#define LOG_INFO \
if(learning::logging::getLevel() <= learning::logging::INFO) \
    learning::logging(__FILE__, __LINE__, learning::logging::INFO).stream()
#define LOG_WARN \
    learning::logging(__FILE__, __LINE__, learning::logging::WARN).stream()
#define LOG_ERROR \
    learning::logging(__FILE__, __LINE__, learning::logging::ERROR).stream()
#define LOG_FATAL \
    learning::logging(__FILE__, __LINE__, learning::logging::FATAL).stream()
#define LOG_SYSERR \
    learning::logging(__FILE__, __LINE__, false).stream()
#define LOG_SYSFATAL \
    learning::logging(__FILE__, __LINE__, true).stream()

} //namespace learning

#endif // LEARNING_BASE_LOGGING_H