#ifndef VIFRAME_H
#define VIFRAME_H

#include <malloc.h>

class VIFrame {
public:
    enum PixelFormat {
        GRAY,
        I420,
        YU16,
        BGR,
        NV21,
    };

	PixelFormat mFormat = NV21;
    unsigned int mSize;
    unsigned int mWidth;
    unsigned int mHeiht;
    unsigned char *mData;
    void release();
	
};

#endif // VIFRAME_H
