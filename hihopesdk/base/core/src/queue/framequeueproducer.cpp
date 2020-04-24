#include "framequeueproducer.h"
#include "../utils/errors.h"

FrameQueueProducer::FrameQueueProducer(FrameQueue *frameQueue) {
    mFrameQueue = frameQueue;
}

int FrameQueueProducer::getFreeSlotLocked() const {
    if (mFrameQueue->mFreeSlots.empty()) {
        return FrameQueue::INVALID_BUFFER_SLOT;
    }

    int slot = mFrameQueue->mFreeSlots.front();
    mFrameQueue->mFreeSlots.pop_front();
    return slot;
}

int FrameQueueProducer::waitForFreeSlotThenRelock(int *found) const {
    bool tryAgain(true);
    while (tryAgain) {
        int dequeuedCount = 0;
        for (int s : mFrameQueue->mActiveSlots) {
            size_t idx = static_cast<size_t>(s);
            if (mFrameQueue->mSlots[idx].mStatus == FrameSlot::Dequeue) {
                ++dequeuedCount;
            }
        }

        if (dequeuedCount >= mFrameQueue->mMaxDequeuedBufferCount) {
            //std::cout << "drop frame" << std::endl;
            return INVALID_OPERATION;
        }

        *found = FrameQueue::INVALID_BUFFER_SLOT;

        const int maxBufferCount = mFrameQueue->getMaxBufferCountLocked();
        bool tooManyBuffers = mFrameQueue->mQueue.size() > static_cast<size_t>(maxBufferCount);
        if (!tooManyBuffers) {
            int slot = getFreeSlotLocked();
            if (slot != FrameQueue::INVALID_BUFFER_SLOT) {
                *found = slot;
            }
        }

        tryAgain = (*found == FrameQueue::INVALID_BUFFER_SLOT) || tooManyBuffers;
        if (tryAgain) {
            mFrameQueue->mDequeueCond.wait(mFrameQueue->mMutex);
        }
    }

    return NO_ERROR;
}

bool FrameQueueProducer::dequeueFrame(int *slot) {

    {
        std::unique_lock<std::mutex> locker(mFrameQueue->mMutex);
        int found = FrameQueue::INVALID_BUFFER_SLOT;
        int ret = waitForFreeSlotThenRelock(&found);
        if (ret != NO_ERROR) {
            std::cout << __FILE__ " ["<< __LINE__ <<  "] "<< "FrameQueueProducer dequeueFrame failed" << std::endl;
            return false;
        }
        if (found == FrameQueue::INVALID_BUFFER_SLOT) {
            std::cout << __FILE__ " ["<< __LINE__ <<  "] "<< "FrameQueueProducer dequeueFrame failed" << std::endl;
            return false;
        }

        mFrameQueue->mSlots[found].mStatus = FrameSlot::Dequeue;
        mFrameQueue->mActiveSlots.insert(found);
        *slot = found;
    }

    return true;
}

bool FrameQueueProducer::requestFrame(int slot, Frame **item) {
    if (slot == FrameQueue::INVALID_BUFFER_SLOT || slot >= 64) {
        std::cout << __FILE__ " ["<< __LINE__ <<  "] "<< "FrameQueueProducer requestFrame failed" << std::endl;
        return false;
    }

    {
        std::unique_lock<std::mutex> locker(mFrameQueue->mMutex);
        if ( mFrameQueue->mSlots[slot].mStatus != FrameSlot::Dequeue) {
            std::cout << __FILE__ " ["<< __LINE__ <<  "] "<< "FrameQueueProducer requestFrame failed" << std::endl;
            return false;
        }
        *item = &(mFrameQueue->mSlots[slot].mFrame);
    }

    return true;
}

bool FrameQueueProducer::queueFrame(int slot) {
    if (slot == FrameQueue::INVALID_BUFFER_SLOT || slot >= 64) {
        std::cout << __FILE__ " ["<< __LINE__ <<  "] "<< "FrameQueueProducer queueFrame failed" << std::endl;
        return false;
    }

    {
        std::unique_lock<std::mutex> locker(mFrameQueue->mMutex);
        if ( mFrameQueue->mSlots[slot].mStatus != FrameSlot::Dequeue) {
            std::cout << __FILE__ " ["<< __LINE__ <<  "] "<< "FrameQueueProducer queueFrame failed" << std::endl;
            return false;
        }

        mFrameQueue->mSlots[slot].mStatus = FrameSlot::Queue;

        FrameItem item;
        item.mSlot = slot;
        item.mFrame = &mFrameQueue->mSlots[slot].mFrame;
        mFrameQueue->mQueue.push_back(item);
        mFrameQueue->mAccquireCond.notify_all();
    }
    return true;
}

