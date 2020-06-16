#ifndef FRAMEQUEUE_H
#define FRAMEQUEUE_H

#include "frameitem.h"
#include "frameslot.h"
#include <array>
#include <mutex>
#include <condition_variable>

static constexpr int NUM_BUFFER_SLOTS = 64;

class FrameQueue
{
public:
    enum {
        INVALID_BUFFER_SLOT = -1
    };

    typedef std::vector<FrameItem> Fifo;

    FrameQueue(int maxAcquiredBufferCount = 2, int maxDequeuedBufferCount = 1);
    int getMaxBufferCountLocked() const;
    std::string info();

    std::mutex mMutex;
    std::condition_variable_any mDequeueCond;
    std::condition_variable_any mAccquireCond;

    Fifo mQueue;
    FrameSlot mSlots[64];

    std::set<int> mActiveSlots;
    std::list<int> mFreeSlots;

    int mMaxAcquiredBufferCount;
    int mMaxDequeuedBufferCount;
    int mDequeueBufferCannotBlock;
 };

#endif // FRAMEQUEUE_H
