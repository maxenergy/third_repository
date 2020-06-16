#ifndef BUFFERQUEUE_H
#define BUFFERQUEUE_H

#include <array>
#include <list>
#include <mutex>
#include <iostream>
#include <condition_variable>

template<typename T>
class BufferSlotT {
public:
    enum {
        Unused,
        Free,
        Dequeue,
        Queue,
        Accquire
    };

    T mObject;
    int mStatus;
};

template<typename T>
class BufferItemT {
public:
    T *mObject;
    int mSlotIdx;
};

template<typename T, std::size_t SIZE>
class BufferQueue {
public:

    typedef BufferSlotT<T> BufferSlot;
    typedef BufferItemT<T> BufferItem;

    BufferSlot mSlots[SIZE];
    std::list<int> mFreeSlots;
    std::list<int> mQueueSlots;

    std::mutex mMutex;
    std::condition_variable_any mFreeCond;
    std::condition_variable_any mQueueCond;

    BufferQueue() {
        for (int i = 0; i < SIZE; i++) {
            mFreeSlots.push_back(i);
        }
    }

    int getFreeSlotLocked() {
        if (mFreeSlots.empty()) {
            return -1;
        }
        int slotIdx = mFreeSlots.front();
        mFreeSlots.pop_front();
        return slotIdx;
    }

    int waitForFreeSlotThenRelock() {
        int slotIdx = -1;
        bool tryAgain(true);
        while (tryAgain) {
            slotIdx  = getFreeSlotLocked();
            tryAgain = (slotIdx == -1);
            if (tryAgain) {
                mFreeCond.wait(mMutex);
            }
        }
        return slotIdx;
    }

    int getQueueSlotLocked() {
        if (mQueueSlots.empty()) {
            return -1;
        }
        int slotIdx = mQueueSlots.front();
        mQueueSlots.pop_front();
        return slotIdx;
    }

    int waitForQueueSlotThenRelock() {
        int slotIdx = -1;
        bool tryAgain(true);
        while (tryAgain) {
            slotIdx  = getQueueSlotLocked();
            tryAgain = (slotIdx == -1);
            if (tryAgain) {
                mQueueCond.wait(mMutex);
            }
        }
        return slotIdx;
    }

    bool dequeue(int *slot) {
        {
            std::unique_lock<std::mutex> locker(mMutex);
            int found = waitForFreeSlotThenRelock();
            mSlots[found].mStatus = BufferSlot::Dequeue;
            *slot = found;
        }
        return true;
    }

    bool request(int slot, T **obj) {
        if (slot == -1 || slot >= SIZE) {
            std::cout << __FILE__ " ["<< __LINE__ <<  "] "<< "request failed" << std::endl;
            return false;
        }

        {
            std::unique_lock<std::mutex> locker(mMutex);
            if (mSlots[slot].mStatus != BufferSlot::Dequeue) {
                std::cout << __FILE__ " ["<< __LINE__ <<  "] " << "request failed" << std::endl;
                return false;
            }
            *obj = &(mSlots[slot].mObject);
        }
        return true;
    }

    bool queue(int slot) {
        if (slot == -1 || slot >= SIZE) {
            std::cout << __FILE__ " ["<< __LINE__ <<  "] "<< "queue failed" << std::endl;
            return false;
        }
        {
            std::unique_lock<std::mutex> locker(mMutex);
            if (mSlots[slot].mStatus != BufferSlot::Dequeue) {
                std::cout << __FILE__ " ["<< __LINE__ <<  "] "<< "queue failed" << std::endl;
                return false;
            }
            mSlots[slot].mStatus = BufferSlot::Queue;
            mQueueSlots.push_back(slot);
            mQueueCond.notify_all();
        }
        return true;
    }

    bool accquire(BufferItem *outItem) {
        {
            std::unique_lock<std::mutex> locker(mMutex);
            int found = waitForQueueSlotThenRelock();
            outItem->mSlotIdx = found;
            outItem->mObject = &(mSlots[found].mObject);
            mSlots[found].mStatus = BufferSlot::Accquire;
        }
        return true;
    }

    bool release(int slot) {
        if (slot == -1 || slot >= SIZE) {
            std::cout << __FILE__ " ["<< __LINE__ <<  "] "<< "release failed" << std::endl;
            return false;
        }
        {
            std::unique_lock<std::mutex> locker(mMutex);
            if (mSlots[slot].mStatus != BufferSlot::Accquire) {
                std::cout << __FILE__ " ["<< __LINE__ <<  "] " << "release failed" << std::endl;
                return false;
            }
            mSlots[slot].mStatus = BufferSlot::Free;
            mFreeSlots.push_back(slot);
            mFreeCond.notify_all();
        }
        return true;
    }
};

#endif // BUFFERQUEUE_H
