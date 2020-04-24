#include "../utils/utils.h"
#include "hi_comm_vb.h"
#include "mpi_vi.h"
#include "mpi_sys.h"
#include "hi_math.h"
#include "hi_common.h"
#include "hi_comm_video.h"
#include "hi_comm_sys.h"
#include "mpi_sys.h"
#include "hi_comm_vb.h"
#include "hi_buffer.h"
#include "mpi_vb.h"
#include "hi_comm_vpss.h"
#include "mpi_vpss.h"
#include "mpi_vgs.h"

#include "mppframe.h"
#include <iostream>

#include "hi_comm_vb.h"
#include "mpi_vi.h"
#include "mpi_sys.h"
#include <iostream>
#include <time.h>
#include "mppcomponent.h"

//static bool viReader_convertFrame(VIDEO_FRAME_S *frame, VIFrame *outFrame) {
//    if (frame->enVideoFormat != VIDEO_FORMAT_LINEAR) {
//        std::cout << "VideoFormat error" << std::endl;
//        return false;
//    }

//    HI_U32 srcSize;
//    HI_U32 dstSize;
//    HI_U32 vuHeight;
//    switch (frame->enPixelFormat) {
//    case PIXEL_FORMAT_YUV_400:
//        vuHeight = 0;
//        outFrame->mFormat = VIFrame::GRAY;
//        srcSize = (frame->u32Height) * (frame->u32Stride[0]);
//        dstSize = (frame->u32Height) * (frame->u32Width);
//        break;
//    case PIXEL_FORMAT_YVU_SEMIPLANAR_420:
//        vuHeight = frame->u32Height/2;
//        outFrame->mFormat = VIFrame::I420;
//        srcSize = (frame->u32Height) * (frame->u32Stride[0]) * 3/2;
//        dstSize = (frame->u32Height) * (frame->u32Width) * 3/2;
//        break;
//    case PIXEL_FORMAT_YVU_SEMIPLANAR_422:
//        vuHeight = frame->u32Height;
//        outFrame->mFormat = VIFrame::YU16;
//        srcSize = (frame->u32Height) * (frame->u32Stride[0]) * 2;
//        dstSize = (frame->u32Height) * (frame->u32Width) * 2;
//        break;
//    default:
//        return false;
//    }

//    unsigned char *srcAddr = (unsigned char *)HI_MPI_SYS_Mmap(frame->u64PhyAddr[0], srcSize);
//    unsigned char *srcYAddr = srcAddr;
//    unsigned char *srcVUAddr = srcAddr + (frame->u32Height) * (frame->u32Stride[0]);

//    unsigned char *dstAddr = (unsigned char*)malloc(dstSize);
//    unsigned char *dstYAddr = dstAddr;
//    unsigned char *dstUAddr = dstYAddr + frame->u32Height * frame->u32Width;
//    unsigned char *dstVAddr = dstUAddr + vuHeight * frame->u32Width / 2;

//    // save Y
//    unsigned int w, h;
//    for (h = 0; h < frame->u32Height; h++) {
//        memcpy(dstYAddr, srcYAddr, frame->u32Width);
//        dstYAddr += frame->u32Width;
//        srcYAddr += frame->u32Stride[0];
//    }

//    // save VU
//    for (h = 0; h < vuHeight; h++)
//    {
//        for (w = 0; w < frame->u32Width / 2; w++)
//        {
//            dstVAddr[w] = srcVUAddr[w*2];
//            dstUAddr[w] = srcVUAddr[w*2+1];
//        }
//        dstVAddr += frame->u32Width / 2;
//        dstUAddr += frame->u32Width / 2;
//        srcVUAddr += frame->u32Stride[1];
//    }

//    outFrame->mData = dstAddr;
//    outFrame->mSize = dstSize;
//    outFrame->mWidth = frame->u32Width;
//    outFrame->mHeiht = frame->u32Height;
//    HI_MPI_SYS_Munmap(srcAddr, srcSize);
//    return true;
//}

//static bool viReader_dump(VIDEO_FRAME_INFO_S &frame) {
//    std::cout << "width: " << frame.stVFrame.u32Width << std::endl;
//    std::cout << "height: " << frame.stVFrame.u32Height << std::endl;
//    //std::cout << "pixel format: " << pixelFormat2String(frame.stVFrame.enPixelFormat) << std::endl;
//    //std::cout << "video format: " << videoFormat2String(frame.stVFrame.enVideoFormat) << std::endl;
//    return true;
//}

//bool viPipeChnReader_init(int Pipe, int Chn) {
//    HI_S32 s32Ret;
//    HI_U32 u32Depth = 2;
//    VI_CHN_ATTR_S stChnAttr;
//    VI_EXT_CHN_ATTR_S stExtChnAttr;
//    if (Chn > VI_MAX_PHY_CHN_NUM - 1) {
//        s32Ret = HI_MPI_VI_GetExtChnAttr(Pipe, Chn, &stExtChnAttr);
//        if (HI_SUCCESS != s32Ret) {
//            printf("[zlj] HI_MPI_VI_GetExtChnAttr Failed: %#x\n", s32Ret);
//            return false;
//        }
//        //u32OrigDepth = stExtChnAttr.u32Depth;
//        stExtChnAttr.u32Depth = u32Depth;
//        stExtChnAttr.stSize.u32Width = 640;
//        stExtChnAttr.stSize.u32Height = 480;
//        s32Ret = HI_MPI_VI_SetExtChnAttr(Pipe, Chn, &stExtChnAttr);
//        if (HI_SUCCESS != s32Ret) {
//            printf("[zlj] HI_MPI_VI_SetExtChnAttr Failed: %#x\n", s32Ret);
//            return false;
//        }
//    } else {
//        s32Ret = HI_MPI_VI_GetChnAttr(Pipe, Chn, &stChnAttr);
//        if (HI_SUCCESS != s32Ret){
//            printf("[zlj] HI_MPI_VI_GetChnAttr Failed: %#x\n", s32Ret);
//            return false;
//        }
//        //u32OrigDepth = stChnAttr.u32Depth;
//        stChnAttr.u32Depth = u32Depth;
//        s32Ret = HI_MPI_VI_SetChnAttr(Pipe, Chn, &stChnAttr);
//        if (HI_SUCCESS != s32Ret) {
//            printf("[zlj] HI_MPI_VI_SetChnAttr Failed: %#x\n", s32Ret);
//            return false;
//        }
//    }
//    return true;
//}

//bool viPipeChnReader_info(int pipe, int chn) {

//    VIDEO_FRAME_INFO_S frame;
//    memset(&frame, 0, sizeof(frame));
//    frame.u32PoolId = VB_INVALID_POOLID;

//    HI_S32 ret = HI_MPI_VI_GetChnFrame(pipe, chn, &frame, 2000);
//    if (ret != HI_SUCCESS) {
//        printf("[zlj] HI_MPI_VI_GetChnFrame Failed: %#x\n", ret);
//        //std::cout << "[zlj] HI_MPI_VI_GetChnFrame Failed: %#x!" << ret << std::endl;
//        return false;
//    }
//    viReader_dump(frame);
//    return true;
//}

//bool viPipeChnReader_getFrame(VIFrame &outFrame, int pipe, int chn, int timeout) {
//    VIDEO_FRAME_INFO_S frame;
//    memset(&frame, 0, sizeof(frame));
//    frame.u32PoolId = VB_INVALID_POOLID;

//    HI_S32 ret = HI_MPI_VI_GetChnFrame(pipe, chn, &frame, timeout);
//    if (ret != HI_SUCCESS) {
//        printf("[zlj] HI_MPI_VI_GetChnFrame Failed: %#x\n", ret);
//    }
//    ret = viReader_convertFrame(&frame.stVFrame, &outFrame);
//    ret = true;
//    HI_MPI_VI_ReleaseChnFrame(pipe, chn, &frame);
//    return ret;
//}


#define MppComponentRegister(type, getChnAttr, setChnAttr, getExtChnAttr, setExtChnAttr, getFrame, releaseFrame) \
    template<> \
    type::GetChnAttr type::sGetChnAttr = getChnAttr; \
    template<> \
    type::SetChnAttr type::sSetChnAttr = setChnAttr; \
    template<> \
    type::GetExtChnAttr type::sGetExtChnAttr = getExtChnAttr; \
    template<> \
    type::SetExtChnAttr type::sSetExtChnAttr = setExtChnAttr; \
    template<> \
    type::GetFrame type::sGetFrame = getFrame; \
    template<> \
    type::ReleaseFrame type::sReleaseFrame = releaseFrame;

MppComponentRegister(MppPipe, HI_MPI_VI_GetChnAttr, HI_MPI_VI_SetChnAttr, HI_MPI_VI_GetExtChnAttr, HI_MPI_VI_SetExtChnAttr, HI_MPI_VI_GetChnFrame, HI_MPI_VI_ReleaseChnFrame)
MppComponentRegister(MppVpss, HI_MPI_VPSS_GetChnAttr, HI_MPI_VPSS_SetChnAttr, HI_MPI_VPSS_GetExtChnAttr, HI_MPI_VPSS_SetExtChnAttr, HI_MPI_VPSS_GetChnFrame, HI_MPI_VPSS_ReleaseChnFrame)
