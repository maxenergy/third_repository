#include "framequeueconsumer.h"
#include "../utils/errors.h"

FrameQueueConsumer::FrameQueueConsumer(FrameQueue *frameQueue) {
    mFrameQueue = frameQueue;
}

int FrameQueueConsumer::waitForQueueSlotThenRelock() const {
    bool tryAgain(true);
    while (tryAgain) {
        int numAcquiredBuffers = 0;
        for (int s : mFrameQueue->mActiveSlots) {
            if (mFrameQueue->mSlots[s].mStatus == FrameSlot::Accquire) {
                ++numAcquiredBuffers;
            }
        }

        if (numAcquiredBuffers >= mFrameQueue->mMaxAcquiredBufferCount) {
            return INVALID_OPERATION;
        }

        tryAgain = mFrameQueue->mQueue.empty();
        if (tryAgain) {
            mFrameQueue->mAccquireCond.wait(mFrameQueue->mMutex);
        }
    }
    return NO_ERROR;
}


bool FrameQueueConsumer::accquire(FrameItem *outItem) {
    {
        std::unique_lock<std::mutex> locker(mFrameQueue->mMutex);
        int ret = waitForQueueSlotThenRelock();
        if (ret != NO_ERROR) {
            std::cout << __FILE__ " ["<< __LINE__ <<  "] "<< "FrameQueueConsumer accquire failed" << std::endl;
            return false;
        }

        FrameQueue::Fifo::iterator front(mFrameQueue->mQueue.begin());
        int slot = front->mSlot;
        outItem->mSlot = slot;
        outItem->mFrame = &mFrameQueue->mSlots[slot].mFrame;
        mFrameQueue->mSlots[slot].mStatus = FrameSlot::Accquire;
        mFrameQueue->mQueue.erase(front);
        mFrameQueue->mDequeueCond.notify_all();
    }
    return true;
}


bool FrameQueueConsumer::release(int slot) {
    if (slot == FrameQueue::INVALID_BUFFER_SLOT || slot >= 64) {
        std::cout << __FILE__ " ["<< __LINE__ <<  "] "<< "FrameQueueConsumer release failed" << std::endl;
        return false;
    }

    {
        std::unique_lock<std::mutex> locker(mFrameQueue->mMutex);
        if ( mFrameQueue->mSlots[slot].mStatus != FrameSlot::Accquire) {
            std::cout << __FILE__ " ["<< __LINE__ <<  "] "<< "FrameQueueConsumer release failed" << std::endl;
            return false;
        }
        mFrameQueue->mSlots[slot].mStatus = FrameSlot::Free;
        mFrameQueue->mActiveSlots.erase(slot);
        mFrameQueue->mFreeSlots.push_back(slot);
        mFrameQueue->mDequeueCond.notify_all();
    }
    return true;
}
