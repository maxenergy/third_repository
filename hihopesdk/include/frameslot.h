#ifndef BUFFERSLOT_H
#define BUFFERSLOT_H

#include "frame.h"

class FrameSlot
{
public:
    enum {
        Unused,
        Free,
        Dequeue,
        Queue,
        Accquire
    };

    FrameSlot();
    std::string statusStr();

    int mStatus;
    Frame mFrame;
};

#endif // BUFFERSLOT_H
