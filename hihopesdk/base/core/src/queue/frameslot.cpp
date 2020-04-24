#include "frameslot.h"

FrameSlot::FrameSlot() : mStatus(Unused)
{

}

std::string FrameSlot::statusStr() {
    switch (mStatus) {
        case Unused:
            return "Unused";
        case Free:
            return "Free";
        case Dequeue:
            return "Dequeue";
        case Queue:
            return "Queue";
        case Accquire:
            return "Accquire";
        default:
            return "error";
    }
}
