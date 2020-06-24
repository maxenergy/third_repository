#include "MppPipe.h"
#include <arm_neon.h>
bool MppPipe::init(int Pipe, int Chn){
return true;
}

bool MppPipe::info(int pipe, int chn) {
    VIDEO_FRAME_INFO_S frame;
    memset(&frame, 0, sizeof(frame));
   // HI_S32 ret = HI_MPI_VI_GetChnFrame(pipe, chn, &frame, 2000);
   
   HI_S32 ret =HI_MPI_VPSS_GetChnFrame(pipe, chn, &frame, 2000);
    if (ret != HI_SUCCESS) {
        printf("[hihope] HI_MPI_VI_GetChnFrame Failed: %#x\n", ret);
        return false;
    }
    dumpFrame(frame);
	
    HI_MPI_VPSS_ReleaseChnFrame(pipe, chn, &frame);
    return true;
}

bool MppPipe::getFrame(VIFrame &outFrame, int pipe, int chn, int timeout) {
    VIDEO_FRAME_INFO_S frame;
    memset(&frame, 0, sizeof(frame));
    frame.u32PoolId = VB_INVALID_POOLID;

    HI_S32 ret =HI_MPI_VPSS_GetChnFrame(pipe, chn, &frame, timeout);
    if (ret != HI_SUCCESS) {
        printf("[hihope]HI_MPI_VI_GetChnFrame Failed: %#x\n", ret);
    }
    ret = convertFrame(&frame.stVFrame, &outFrame);
    HI_MPI_VPSS_ReleaseChnFrame(pipe, chn, &frame);
    return ret;
}

bool MppPipe::getFrame(VIFrame &outFrame_bgr,VIFrame &outFrame_ir, int pipe_brg,int pipe_ir, int chn, int timeout) {
    VIDEO_FRAME_INFO_S frame_brg;
	VIDEO_FRAME_INFO_S frame_ir;
    memset(&frame_ir, 0, sizeof(frame_ir));
    memset(&frame_brg, 0, sizeof(frame_brg));
    frame_brg.u32PoolId = VB_INVALID_POOLID;
    frame_ir.u32PoolId = VB_INVALID_POOLID;

    HI_S32 ret = HI_MPI_VI_GetChnFrame(pipe_brg, chn, &frame_brg, timeout);
    if (ret != HI_SUCCESS) {
        printf("[hihope]HI_MPI_VI_GetChnFrame Failed: %#x\n", ret);
    }
    ret = HI_MPI_VI_GetChnFrame(pipe_ir, chn, &frame_ir, timeout);
    if (ret != HI_SUCCESS) {
        printf("[hihope]HI_MPI_VI_GetChnFrame Failed: %#x\n", ret);
    }
    ret = convertFrame(&frame_brg.stVFrame, &outFrame_bgr);
    HI_MPI_VPSS_ReleaseChnFrame(pipe_brg, chn, &frame_brg);
	
	ret = convertFrame(&frame_ir.stVFrame, &outFrame_ir);
    HI_MPI_VPSS_ReleaseChnFrame(pipe_ir, chn, &frame_ir);
    return ret;
}



bool MppPipe::dumpFrame(VIDEO_FRAME_INFO_S &frame) {
    std::cout << "width: " << frame.stVFrame.u32Width << std::endl;
    std::cout << "height: " << frame.stVFrame.u32Height << std::endl;
    std::cout << "pixel format: " << pixelFormat2String(frame.stVFrame.enPixelFormat) << std::endl;
    std::cout << "video format: " << videoFormat2String(frame.stVFrame.enVideoFormat) << std::endl;
    return true;
}
bool MppPipe::convertFrame(VIDEO_FRAME_S *frame, VIFrame *outFrame) {
    if (frame->enVideoFormat != VIDEO_FORMAT_LINEAR) {
        std::cout << "VideoFormat error" << std::endl;
        return false;
    }

    HI_U32 srcSize;
    HI_U32 dstSize;
    HI_U32 vuHeight;
    switch (frame->enPixelFormat) {
    case PIXEL_FORMAT_YUV_400:
        vuHeight = 0;
        outFrame->mFormat = VIFrame::GRAY;
        srcSize = (frame->u32Height) * (frame->u32Stride[0]);
        dstSize = (frame->u32Height) * (frame->u32Width);
        break;
    case PIXEL_FORMAT_YVU_SEMIPLANAR_420:
        vuHeight = frame->u32Height/2;
        outFrame->mFormat = VIFrame::I420;
        srcSize = (frame->u32Height) * (frame->u32Stride[0]) * 3/2;
        dstSize = (frame->u32Height) * (frame->u32Width) * 3/2;
        break;
    case PIXEL_FORMAT_YVU_SEMIPLANAR_422:
        vuHeight = frame->u32Height;
        outFrame->mFormat = VIFrame::YU16;
        srcSize = (frame->u32Height) * (frame->u32Stride[0]) * 2;
        dstSize = (frame->u32Height) * (frame->u32Width) * 2;
        break;
    default:
        return false;
    }

    unsigned char *srcAddr = (unsigned char *)HI_MPI_SYS_Mmap(frame->u64PhyAddr[0], srcSize);
    unsigned char *srcYAddr = srcAddr;
    unsigned char *srcVUAddr = srcAddr + (frame->u32Height) * (frame->u32Stride[0]);

    unsigned char *dstAddr = (unsigned char*)malloc(dstSize);
    unsigned char *dstYAddr = dstAddr;
	unsigned char *dstVUAddr = dstYAddr + frame->u32Height * frame->u32Width;
	unsigned int w, h;
    for (h = 0; h < frame->u32Height; h++) {
    	memcpy(dstYAddr, srcYAddr, frame->u32Width);
	    dstYAddr += frame->u32Width;
	    srcYAddr += frame->u32Stride[0];
		if(h < vuHeight){
			memcpy(dstVUAddr, srcVUAddr, frame->u32Width);
		    dstVUAddr += frame->u32Width;
		    srcVUAddr += frame->u32Stride[1];
		}
    }
#if 0
    // save Y
    unsigned int w, h;
    for (h = 0; h < frame->u32Height; h++) {
        memcpy(dstYAddr, srcYAddr, frame->u32Width);
        dstYAddr += frame->u32Width;
        srcYAddr += frame->u32Stride[0];
    }

    // save VU
	for (h = 0; h < vuHeight-1; h++)
		{
			for (w = 0; w < frame->u32Width / 2; )
			{
        	   uint8x16x2_t vu_data;
        	   vu_data = vld2q_u8((uint8_t*)(srcVUAddr+w*2));
        	   vst1q_u8((uint8_t*)(dstVAddr+w), vu_data.val[0]);
        	   vst1q_u8((uint8_t*)(dstUAddr+w), vu_data.val[1]);
        	   w=w+16;
			}
			dstVAddr += frame->u32Width / 2;
			dstUAddr += frame->u32Width / 2;
			srcVUAddr += frame->u32Stride[1];
		}
	
		for (w = 0; w < frame->u32Width / 2; w++)
		{
			dstVAddr[w] = srcVUAddr[w*2];
			dstUAddr[w] = srcVUAddr[w*2+1];
		}
#endif
    outFrame->mData = dstAddr;
    outFrame->mSize = dstSize;
    outFrame->mWidth = frame->u32Width;
    outFrame->mHeiht = frame->u32Height;
    HI_MPI_SYS_Munmap(srcAddr, srcSize);

    return true;
}


