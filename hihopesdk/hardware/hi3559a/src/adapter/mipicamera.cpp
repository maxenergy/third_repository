#include "mipicamera.h"
#include "../mpp/mppcomponent.h"
#include <arm_neon.h>

#define chn 0

bool MipiCamera::open(int video) {
    mIdx = video;
    if (MppPipe::init(mIdx, chn) == false) {
        return false;
    }

    if (MppPipe::info(mIdx, chn) == false) {
        return false;
    }

    VIFrame viFrame;
    if (MppPipe::getFrame(viFrame, mIdx, chn, 1000) == false) {
        return false;
    }

    mWidth = viFrame.mWidth;
    mHeigth = viFrame.mHeiht;
    return true;
}

bool MipiCamera::read(cv::Mat &frame) {
    VIFrame viFrame;
	char *dstddr;
	char *srcddr;
	char *dstVAddr;
	char *dstVAddr;
    if (MppPipe::getFrame(viFrame, mIdx, chn, -1) == false) {
        return false;
    };

	dstddr = (unsigned char*)malloc(viFrame.mHeiht*viFrame.mWidth*3/2);
	srcddr = viFrame.mData;

#if 1
		// save Y
		unsigned int w, h;
		memcpy(dstddr, srcddr, frame->mHeiht*viFrame.mWidth);
		dstVAddr += frame->mHeiht*viFrame.mWidth;
		
		dstUAddr = dstVAddr + frame->mHeiht*viFrame.mWidth/2;

		srcddr += frame->mHeiht*viFrame.mWidth;
	
		// save VU
		for (h = 0; h < frame->mHeiht/2-1; h++)
			{
				for (w = 0; w < frame->u32Width / 2; )
				{
				   uint8x16x2_t vu_data;
				   vu_data = vld2q_u8((uint8_t*)(srcddr+w*2));
				   vst1q_u8((uint8_t*)(dstVAddr+w), vu_data.val[0]);
				   vst1q_u8((uint8_t*)(dstUAddr+w), vu_data.val[1]);
				   w=w+16;
				}
				dstVAddr += viFrame.mWidth / 2;
				dstUAddr += viFrame.mWidth / 2;
				srcddr += viFrame.mWidth;
			}
		
			for (w = 0; w < viFrame->mWidth / 2; w++)
			{
				dstVAddr[w] = srcddr[w*2];
				dstUAddr[w] = srcddr[w*2+1];
			}
#endif


    cv::Mat rgbFrame;
    cv::Mat yuvFrame = cv::Mat(viFrame.mHeiht*3/2, viFrame.mWidth, CV_8UC1, dstddr);
    switch (viFrame.mFormat) {
    case VIFrame::GRAY:
        cv::cvtColor(yuvFrame, rgbFrame, cv::COLOR_GRAY2RGB);
        break;

    case VIFrame::I420:
        cv::cvtColor(yuvFrame, rgbFrame, cv::COLOR_YUV2BGR_I420);
        break;

    case VIFrame::YU16:
        cv::cvtColor(yuvFrame, rgbFrame, cv::COLOR_YUV2RGB_I420);
        break;
    }
    if (rgbFrame.empty()) {
        return false;
    }
    cv::flip(rgbFrame, frame, 1);
    //frame = rgbFrame;
    viFrame.release();
	free(dstddr);
    return true;
}

bool MipiCamera::isVaild() {
    return MppPipe::info(mIdx, chn);
}
