#ifndef LEARNING_BASE_STRINGPIECE_H
#define LEARNING_BASE_STRINGPIECE_H

#include <learning/base/copyable.h>

#include <string_view>
#include <string>
#include <cstring>

namespace learning{

class stringArg: public learning::copyable
{
public:
    stringArg(const char* data)
      : data_(data) {}
    stringArg(const std::string& data)
      : data_(data.data()) {}
    ~stringArg() = default;

    const char* data() const
    { return data_; }

private:
    const char* data_;
};

using stringPiece = std::string_view;

} // namespace learning

#endif // LEARNING_BASE_STRINGPIECE_H