#include "mipicamera.h"
#include "../mpp/MppPipe.h"
#include <stdio.h>
#include <arm_neon.h>
#define chn 0
bool MipiCamera::open(int video) {
    mIdx = video;
    if (MppPipe::init(mIdx, chn) == false) {
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

bool MipiCamera::read(VIFrame &viFrame) {
    if (MppPipe::getFrame(viFrame, mIdx, chn, -1) == false) {
        return false;
    };
    return true;
}

bool MipiCamera::read(int vpss_channel,VIFrame &viFrame) {
    if (MppPipe::getFrame(viFrame, mIdx, vpss_channel, -1) == false) {
        return false;
    };
    return true;
}


bool MipiCamera::read(VIFrame &viFrame_bgr,VIFrame &viFrame_ir){
	if (MppPipe::getFrame(viFrame_bgr,viFrame_ir, 0, 2, chn, -1) == false) {
			return false;
		};
	 return true;
}



bool MipiCamera::read(cv::Mat &frame) {
    VIFrame viFrame;
	unsigned char *dstddr;
	unsigned char *srcddr;
	unsigned char *dstVAddr;
    unsigned char *dstUAddr;
	HI_U32 vuHeight;
    if (MppPipe::getFrame(viFrame, mIdx, chn, -1) == false) {
        return false;
    };
	dstddr = (unsigned char*)malloc(viFrame.mHeiht*viFrame.mWidth*3/2);
	srcddr = viFrame.mData;
	vuHeight = viFrame.mHeiht/2;

#if 1
		// save Y
		unsigned int w, h;
		memcpy(dstddr, srcddr, viFrame.mHeiht*viFrame.mWidth);
		
		dstUAddr = dstddr + viFrame.mHeiht*viFrame.mWidth;
		
		dstVAddr = dstUAddr + vuHeight*viFrame.mWidth/2;

        srcddr += viFrame.mHeiht*viFrame.mWidth;
	
		// save VU
        for (h = 0; h < vuHeight-1; h++)
		{
            for (w = 0; w < viFrame.mWidth/2;)
			{
			   uint8x16x2_t vu_data;
			   vu_data = vld2q_u8((uint8_t*)(srcddr+w*2));
			   vst1q_u8((uint8_t*)(dstVAddr+w), vu_data.val[0]);
			   vst1q_u8((uint8_t*)(dstUAddr+w), vu_data.val[1]);
			   w = w + 16;
			}
			dstVAddr += viFrame.mWidth / 2;
			dstUAddr += viFrame.mWidth / 2;
			srcddr += viFrame.mWidth;
		}
		
            for (w = 0; w < vuHeight / 2; w++)
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
    frame = rgbFrame;
    viFrame.release();
	free(dstddr);
    return true;
}


bool MipiCamera::isVaild() {
    return MppPipe::info(mIdx, chn);
}
