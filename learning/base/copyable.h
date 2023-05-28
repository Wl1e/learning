#ifndef LEARNING_BASE_COPYABLE_H
#define LEARNING_BASE_COPYABLE_H

namespace learning
{

class copyable
{
public:
    copyable() = default;
    ~copyable() = default;
    copyable(const copyable&) = default;
};

} // namespace learning

#endif // LEARNING_BASE_COPYABLE_H