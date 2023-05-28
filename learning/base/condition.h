#ifndef LEARNING_BASE_CONDITION_H
#define LEARNING_BASE_CONDITION_H

#include <learning/base/noncopyable.h>
#include <learning/base/mutex.h>
#include <stdint.h>

namespace learning {

class condition: noncopyable
{
public:
    condition(mutex& mutex)
      : mutex_(mutex)
    {
        assert(pthread_cond_init(&cond_, nullptr) == 0);
    }
    ~condition()
    {
        assert(pthread_cond_destroy(&cond_) == 0);
    }
    void wait()
    { assert(pthread_cond_wait(&cond_, mutex_.getMutex()) == 0); }
    void waitTime(double second)
    {
        
        const int NS = 1000000000;
        timespec tim;
        clock_gettime(CLOCK_REALTIME, &tim);
        int64_t NStime = static_cast<int64_t>(NS*second);
        tim.tv_sec = static_cast<time_t>((tim.tv_sec+NStime)/NS);
        tim.tv_nsec = static_cast<long>((tim.tv_nsec+NStime)%NS);

        assert(pthread_cond_timedwait(&cond_, mutex_.getMutex(), &tim) == 0);
    }
    void notifyOne()
    {
        assert(pthread_cond_signal(&cond_) == 0);
    }
    void notifyAll()
    {
        assert(pthread_cond_broadcast(&cond_) == 0);
    }

private:
    mutex& mutex_;
    pthread_cond_t cond_;
};

} // namespace learning

#endif // LEARNING_BASE_CONDITION_H