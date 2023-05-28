#ifndef LEARNING_BASE_NONCOPYABLE_H
#define LEARNING_BASE_NONCOPYABLE_H

namespace learning
{

class noncopyable
{
public:
    noncopyable(const noncopyable&) = delete;
    void operator=(const noncopyable&) = delete;

protected:
    noncopyable() = default;
    ~noncopyable() = default;
};

} // namespace learning

#endif // LEARNING_BASE_NONCOPYABLE_H