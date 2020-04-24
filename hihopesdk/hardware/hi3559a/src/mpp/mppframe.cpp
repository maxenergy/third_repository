#include "mppframe.h"

void VIFrame::release() {
    if (mData != nullptr) {
        free(mData);
    }
}
