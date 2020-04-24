#ifndef VIFRAME_H
#define VIFRAME_H

#include <malloc.h>

class VIFrame {
public:
    enum PixelFormat {
        GRAY,
        I420,
        YU16,
    };

    PixelFormat mFormat;
    unsigned int mSize;
    unsigned int mWidth;
    unsigned int mHeiht;
    unsigned char *mData;
    void release();
	bool empty();
};

#endif // VIFRAME_H
