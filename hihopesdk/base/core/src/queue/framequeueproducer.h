#ifndef FRAMEQUEUEPRODUCER_H
#define FRAMEQUEUEPRODUCER_H

#include "frameitem.h"
#include "framequeue.h"

class FrameQueueProducer
{
public:
    FrameQueueProducer(FrameQueue *frameQueue);

    bool dequeueFrame(int *slot);
    bool requestFrame(int slot, Frame **item);
    bool queueFrame(int slot);

private:
    FrameQueue *mFrameQueue;

    int getFreeSlotLocked() const;
    int waitForFreeSlotThenRelock(int *found) const;
};

#endif // FRAMEQUEUEPRODUCER_H
