#include "mppframe.h"

void VIFrame::release() {
    if (mData != nullptr) {
        free(mData);
		mData = nullptr;
    }
}
bool VIFrame::empty()
    {
        return mData == nullptr;
    }
