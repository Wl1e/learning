#include <learning/net/timerHeap.h>

using namespace learning;
using namespace learning::net;

void timeHeap::add(timeStamp time,
                   int interval,
                   bool repeat,
                   const net::timerCallback& cb)
{

}

void timeHeap::_moveUp(size_t x)
{
    assert(x < heap_.size());
    size_t parent = (x-1)/2;
    size_t child = x;
    while(child != 0) {
        if(heap_[parent].getTime() <= heap_[child].getTime()) {
            break;
        }
        _swapNode(parent, child);
        child = parent;
        parent = (child-1)/2;
    }
}
void timeHeap::_moveDown(size_t x)
{
    assert(x < heap_.size());
    size_t parent = x;
    size_t child = 2*x+1;
    while(child < heap_.size()) {
        if(child + 1 < heap_.size() && heap_[child+1].getTime() < heap_[child].getTime()) {
            ++child;
        }
        if(heap_[child].getTime() >= heap_[parent].getTime()) {
            break;
        }
        _swapNode(parent, child);
        parent = child;
        child = 2*parent+1;
    }
}