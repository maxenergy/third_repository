#include "framequeue.h"
#include <sstream>

FrameQueue::FrameQueue(int maxAcquiredBufferCount, int maxDequeuedBufferCount) :
    mMaxAcquiredBufferCount(maxAcquiredBufferCount),
    mMaxDequeuedBufferCount(maxDequeuedBufferCount) {

    int numStartingBuffers = getMaxBufferCountLocked();
    for (int s = 0; s < numStartingBuffers; s++) {
        mFreeSlots.push_front(s);
    }
}

int FrameQueue::getMaxBufferCountLocked() const {
    return mMaxAcquiredBufferCount + mMaxDequeuedBufferCount;
}

std::string FrameQueue::info() {
    std::stringstream ss;
    std::list<int> slots;

    for (auto iter = mFreeSlots.begin();  iter != mFreeSlots.end(); iter++) {
        slots.push_back(*iter);
    }
    for (auto iter = mActiveSlots.begin();  iter != mActiveSlots.end(); iter++) {
        slots.push_back(*iter);
    }

    ss << "slot info :" << std::endl;
    for (auto iter = slots.begin();  iter != slots.end(); iter++) {
        ss << "\t";
        ss << "{";
        ss << "idx: " << *iter <<  ", ";
        ss << "status: " << mSlots[*iter].statusStr() << ", ";
        ss << "frame width: " << mSlots[*iter].mFrame.mFrameData.cols << ", ";
        ss << "frame height: " << mSlots[*iter].mFrame.mFrameData.rows << ", ";
        ss << "}" ;
        ss << std::endl;
    }
    //std::cout << ss.str();
    return ss.str();
}

