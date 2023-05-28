#ifndef LEARNING_NET_TIMERHEAP_H
#define LEARNING_NET_TIMERHEAP_H

#include <learning/base/noncopyable.h>
#include <learning/net/timer.h>

#include <vector>
#include <assert.h>

namespace learning {
namespace net {

static const size_t estimateSize = 1024;

class timeHeap: noncopyable
{
public:
    timeHeap()
    {
        heap_.reserve(estimateSize);
    }
    ~timeHeap()
    {
        tick();
        /*  需要干其他事吗, 其他未到期连接是否由时间堆断开, 还是server统一调用closeCallback */
    }

    void add(timeStamp time,
             int interval,
             bool repeat,
             const net::timerCallback& pChannel);
    void pop()
    { _del(0); }

    void tick()
    {
        while(!empty()) {
            timer& it = heap_.front();
            if(it.getTime() > timeStamp::now()) {
                break;
            }
            pop();
        }
    }

    int getNextTime()
    {
        tick();
        int ret = -1;
        if(!heap_.empty()) {
            timeStamp top = heap_.front().getTime();
            if(top.valid()) {
                ret = top.MSSinceEpoch();
            }
        }
        return ret;
    }
    size_t size() const
    { return heap_.size(); }
    bool empty() const
    { return size() == 0; }

private:
    void _del(size_t x) /* delete by index */
    {
        assert(!empty() && x < heap_.size());
        if(x == heap_.size() - 1) {
            heap_.pop_back();
        }
        else {
            _swapNode(x, heap_.size()-1);
            heap_.pop_back();
            _moveDown(x);
        }
    }
    void _moveUp(size_t);
    void _moveDown(size_t);
    void _swapNode(size_t lhs, size_t rhs)
    {
        assert(lhs < heap_.size());
        assert(rhs < heap_.size());
        // std::swap(heap_[lhs], heap_[rhs]);
    }

private:
    std::vector<timer> heap_;
};

} // namespace net
} // namespace learning

#endif // LEARNING_NET_TIMERHEAP_H