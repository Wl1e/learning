#ifndef LEARNING_BASE_MUTEX_H
#define LEARNING_BASE_MUTEX_H

#include <pthread.h>
#include <assert.h>

#include <learning/base/currentThread.h>

namespace learning{

#if defined(clang) && (!defined(SWIG))
    #define THREAD_ANNOTATION_ATTRITUTE(x) __attribute__(x)
#else
    #define THREAD_ANNOTATION_ATTRITUTE(x)
#endif

#define GUARDED_BY(x) \
    THREAD_ANNOTATION_ATTRITUTE(guarded_by(x))
#define REQUIRES(...) \
    THREAD_ANNOTATION_ATTRITUTE(requires_capability(__VA_ARGS__))
#define ACQUIRE(...) \
    THREAD_ANNOTATION_ATTRITUTE(acquire_capability(__VA_ARGS__))

class mutex
{
public:
    mutex()
    {
        assert(pthread_mutex_init(&mutex_, nullptr) == 0);
    }
    ~mutex()
    {
        assert(holder_ == 0);
        assert(pthread_mutex_destroy(&mutex_) == 0);
    }
    void lock()
    {
        _assignHolder();
        assert(pthread_mutex_lock(&mutex_) == 0);
    }
    void unlock()
    {
        _unassignHolder();
        assert(pthread_mutex_unlock(&mutex_) == 0);
    }
    pthread_mutex_t* getMutex()
    { return &mutex_; }

private:
    void _assignHolder()
    { holder_ = currentThread::tid(); }
    void _unassignHolder()
    { holder_ = 0; }

private:
    pthread_mutex_t mutex_;
    pid_t holder_;
};

class mutexGuard
{
public:
    explicit mutexGuard(mutex& mutex) ACQUIRE(mutex)
      : mutex_(mutex)
    {
        mutex_.lock();
    }
    ~mutexGuard()
    {
        mutex_.unlock();
    }

private:
    mutex& mutex_;
};

#define mutexGuard(x) error "miss object name"

} // namespace learning

#endif //LEARNING_BASE_MUTEX_H