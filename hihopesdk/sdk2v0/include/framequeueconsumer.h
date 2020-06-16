#ifndef FRAMEQUEUECONSUMER_H
#define FRAMEQUEUECONSUMER_H

#include "frameitem.h"
#include "framequeue.h"

class FrameQueueConsumer
{
public:
    FrameQueueConsumer(FrameQueue *frameQueue);
    bool release(int slot);
    bool accquire(FrameItem *);

private:
    int waitForQueueSlotThenRelock() const;
    FrameQueue *mFrameQueue;
};

#endif // FRAMEQUEUECONSUMER_H
