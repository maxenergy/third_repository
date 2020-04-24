#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/prctl.h>
#include <math.h>

#include "hi_common.h"
#include "hi_comm_sys.h"
#include "sample_comm.h"
#include "sample_comm_dpu.h"
#include "sample_dpu_main.h"

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#define DPU_BIND_VPSS_BUG  1
static SAMPLE_DPU_CONFIG_S s_stSampleDpuConfig;
static SAMPLE_VI_CONFIG_S s_stSampleViConfig;

HI_S32 SAMPLE_DPU_Bind(DPU_RECT_GRP DpuRectGrp, DPU_MATCH_GRP DpuMatchGrp, HI_S32 s32PipeNum)
{
    VI_PIPE  ViPipe;
    VI_CHN   ViChn = 0;
    HI_S32   s32Ret = HI_SUCCESS;
    VPSS_GRP VpssGrp = 0;
    VPSS_CHN VpssChn = 0;
    DPU_RECT_PIPE DpuRectPipe = 0;
    HI_S32 i;

    for (i = 0; i < s32PipeNum; i++)
    {
        ViPipe = i;
        VpssGrp = i;
        s32Ret = SAMPLE_COMM_VI_Bind_VPSS(ViPipe, ViChn, VpssGrp);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("VI bind VPSS failed with %#x!", s32Ret);
            return HI_FAILURE;
        }
    }
#if DPU_BIND_VPSS_BUG
#else
    for (i = 0; i < s32PipeNum; i++)
    {
        VpssGrp = i;
        VpssChn = 0;
        DpuRectPipe = i;
		printf("bind VpssGrp %d ->DpuRectPipe %d \n",VpssGrp,DpuRectPipe);
        s32Ret = SAMPLE_COMM_DPU_VPSS_Bind_RECT(VpssGrp, VpssChn, DpuRectGrp, DpuRectPipe);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("VPSS bind RECT failed with %#x!\n", s32Ret);
            return HI_FAILURE;
        }
    }

    s32Ret = SAMPLE_COMM_DPU_RECT_Bind_MATCH(DpuRectGrp, DpuMatchGrp);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("RECT bind MATCH failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
#endif
    return HI_SUCCESS;
}

extern HI_S32 SAMPLE_COMM_DPU_WriteFrame2File_yuv420p(FILE *pFp, HI_U32 u32EleSize, VIDEO_FRAME_INFO_S *pstVFrameInfo);


HI_S32 SAMPLE_VI_VPSS_UnBind(HI_S32 s32PipeNum)
{
    VI_PIPE  ViPipe;
    VI_CHN   ViChn   = 0;
    HI_S32   s32Ret  = HI_SUCCESS;
    HI_U32   i;
    VPSS_GRP VpssGrp = 0;
    VPSS_CHN VpssChn = 0;
    DPU_RECT_PIPE DpuRectPipe = 0;
    for (i = 0; i < s32PipeNum; i++)
    {
        ViPipe = i;
        VpssGrp = i;
        s32Ret = SAMPLE_COMM_VI_UnBind_VPSS(ViPipe, ViChn, VpssGrp);

        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("VI unbind VPSS failed with %#x", s32Ret);
            return HI_FAILURE;
        }
    }
    
    return HI_SUCCESS;
}



HI_S32 SAMPLE_DPU_UnBind(DPU_RECT_GRP DpuRectGrp, DPU_MATCH_GRP DpuMatchGrp, HI_S32 s32PipeNum)
{
    VI_PIPE  ViPipe;
    VI_CHN   ViChn   = 0;
    HI_S32   s32Ret  = HI_SUCCESS;
    HI_U32   i;
    VPSS_GRP VpssGrp = 0;
    VPSS_CHN VpssChn = 0;
    DPU_RECT_PIPE DpuRectPipe = 0;
#if DPU_BIND_VPSS_BUG
#else

    s32Ret = SAMPLE_COMM_DPU_RECT_UnBind_MATCH(DpuRectGrp, DpuMatchGrp);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("RECT unbind MATCH failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    for (i = 0; i < s32PipeNum; i++)
    {
        VpssGrp = i;
        VpssChn = 0;
        DpuRectPipe = i;
		printf("unbind VpssGrp %d ->DpuRectPipe %d \n",VpssGrp,DpuRectPipe);
        s32Ret = SAMPLE_COMM_DPU_VPSS_UnBind_RECT(VpssGrp, VpssChn, DpuRectGrp, DpuRectPipe);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("VPSS unbind RECT failed with %#x!\n", s32Ret);
            return HI_FAILURE;
        }
    }
#endif
    for (i = 0; i < s32PipeNum; i++)
    {
        ViPipe = i;
        VpssGrp = i;
        s32Ret = SAMPLE_COMM_VI_UnBind_VPSS(ViPipe, ViChn, VpssGrp);

        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("VI unbind VPSS failed with %#x", s32Ret);
            return HI_FAILURE;
        }
    }
    
    return HI_SUCCESS;
}

#define HAS_DEPTH_BUF 0
#define RETURN_LEFT_RGB 1
int dpu_read(char* rgb_buf,char* depth_buf)
{
    HI_S32 s32MilliSec = 2000;
    HI_S32 s32Ret = HI_SUCCESS;
	VIDEO_FRAME_INFO_S stDpuRgbDstFrame_L;
	VIDEO_FRAME_INFO_S srcRECTDstFrame_L,srcRECTDstFrame_R;
	VIDEO_FRAME_INFO_S dstRECTDstFrame_L,dstRECTDstFrame_R;
	int i =0;
	VI_PIPE ViPipe =0;
	VI_CHN vi_chn=0;
#if 0
s32Ret = HI_MPI_DPU_RECT_GetFrame(0,&srcRECTDstFrame_L,&srcRECTDstFrame_R,&dstRECTDstFrame_L, &dstRECTDstFrame_R, s32MilliSec);
			if(s32Ret != HI_SUCCESS)
			{
				SAMPLE_PRT("get rect frame failed for %#x! pipe %d\n", s32Ret,ViPipe);
			}else{
				
			}
		    HI_MPI_DPU_RECT_ReleaseFrame(0,&srcRECTDstFrame_L,&srcRECTDstFrame_R,&dstRECTDstFrame_L, &dstRECTDstFrame_R);
#endif
#if RETURN_LEFT_RGB
				ViPipe = 0;
                clock_t start = clock();
                s32Ret =  HI_MPI_VPSS_GetChnFrame(ViPipe, vi_chn, &stDpuRgbDstFrame_L, s32MilliSec);
                clock_t end = clock();
                SAMPLE_PRT("HI_MPI_VPSS_GetChnFrame %lf", (end-start)/1000.0f);

				if(s32Ret != HI_SUCCESS)
				{
					SAMPLE_PRT("get vi	frame failed for %#x! pipe %d\n", s32Ret,ViPipe);
					return s32Ret;
				}
                //s32Ret = SAMPLE_COMM_DPU_WriteFrame2buf_yuv420p(rgb_buf, sizeof(HI_U8),&stDpuRgbDstFrame_L);
                s32Ret = HI_MPI_VPSS_ReleaseChnFrame(ViPipe, vi_chn, &stDpuRgbDstFrame_L) ;
                return -1;
				
				if(s32Ret != HI_SUCCESS)
				{
					SAMPLE_PRT("Release vi frame failed for %#x!\n", s32Ret);
					return s32Ret;
				}
#else
				ViPipe = 1;
				s32Ret =  HI_MPI_VPSS_GetChnFrame(ViPipe, vi_chn, &stDpuRgbDstFrame_L, s32MilliSec); 
				if(s32Ret != HI_SUCCESS)
				{
					SAMPLE_PRT("get vi	frame failed for %#x! pipe %d\n", s32Ret,ViPipe);
					return s32Ret;
				}

				s32Ret = SAMPLE_COMM_DPU_WriteFrame2buf_yuv420p(rgb_buf, sizeof(HI_U8),
							&stDpuRgbDstFrame_L);
				
				s32Ret = HI_MPI_VPSS_ReleaseChnFrame(ViPipe, vi_chn, &stDpuRgbDstFrame_L) ; 

				if(s32Ret != HI_SUCCESS)
				{
					SAMPLE_PRT("Release vi frame failed for %#x!\n", s32Ret);
					return s32Ret;
				}

#endif
#if HAS_DEPTH_BUF	
        s32Ret = HI_MPI_DPU_MATCH_GetFrame(s_stSampleDpuConfig.DpuMatchGrp, 
                    &s_stSampleDpuConfig.astDpuMatchSrcFrame[DPU_MATCH_LEFT_PIPE],
                    &s_stSampleDpuConfig.astDpuMatchSrcFrame[DPU_MATCH_RIGHT_PIPE], 
                    &s_stSampleDpuConfig.stDpuMatchDstFrame, s32MilliSec);
        if(s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("get dpu match frame failed for %#x!\n", s32Ret);
			return;
        }
		
        /* use match frame to do something */        
        s32Ret = SAMPLE_COMM_DPU_WriteFrame2buf_depth(depth_buf, sizeof(HI_U16),
                    &s_stSampleDpuConfig.stDpuMatchDstFrame);
		
        if(s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("gSAMPLE_COMM_DPU_WriteFrame2File failed for %#x!\n", s32Ret);
        }

        /* do something for match frame */
        s32Ret = HI_MPI_DPU_MATCH_ReleaseFrame(s_stSampleDpuConfig.DpuMatchGrp, 
                    &s_stSampleDpuConfig.astDpuMatchSrcFrame[DPU_MATCH_LEFT_PIPE],
                    &s_stSampleDpuConfig.astDpuMatchSrcFrame[DPU_MATCH_RIGHT_PIPE],
                    &s_stSampleDpuConfig.stDpuMatchDstFrame);
        if(s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("get dpu match frame failed for %#x!\n", s32Ret);
        }
#endif
return s32Ret;
}
/*
u32MaxPoolCnt 256
stVbConfig.astCommPool[0]: u32BlkCnt 20,u64BlkSize 6220800
stVbConfig.astCommPool[1]: u32BlkCnt 16,u64BlkSize 3686400
stVbConfig.astCommPool[2]: u32BlkCnt 16,u64BlkSize 8294400
stVbConfig.astCommPool[3]: u32BlkCnt 20,u64BlkSize 2764800

*/
int dpu_get_mem(VB_CONFIG_S *stVbConfig)
{
	stVbConfig->u32MaxPoolCnt			 = 256;

	stVbConfig->astCommPool[0].u32BlkCnt  = 20;
	stVbConfig->astCommPool[0].u64BlkSize = 6220800;
	
	stVbConfig->astCommPool[1].u32BlkCnt  = 16;
	stVbConfig->astCommPool[1].u64BlkSize = 3686400;
	
	stVbConfig->astCommPool[2].u32BlkCnt  =16;
	stVbConfig->astCommPool[2].u64BlkSize = 8294400;
	
	stVbConfig->astCommPool[3].u32BlkCnt  = 20;
	stVbConfig->astCommPool[3].u64BlkSize = 2764800;
	return 0;
}

/* This case only for function design reference */
HI_S32 dpu_init(int left_id,int right_id, int need_sysinit)
{
    HI_U32               u32ViChnCnt = 2;
    SIZE_S               stSize;
    VB_CONFIG_S          stVbConfig;
    PIC_SIZE_E           enPicSize = 0;
    HI_S32               s32Ret = HI_SUCCESS;
    HI_S32               s32ViPipeNum = 2;
    HI_S32               s32VpssGrpNum = 2;
    HI_S32               s32VpssGrpNumTmp = 0;
    HI_S32               i;
    VPSS_GRP             VpssGrp = 0;
    VPSS_GRP_ATTR_S      stVpssGrpAttr;
    VPSS_CHN_ATTR_S      astVpssChnAttr[VPSS_MAX_PHY_CHN_NUM];
    HI_BOOL              abChnEn[VPSS_MAX_PHY_CHN_NUM] = {1, 0, 0, 0};
    
    HI_S32               s32DpuRectPipeNum = 0;
    HI_CHAR *apcMapFileName[DPU_RECT_MAX_PIPE_NUM] = {"1280x720_LeftMap.dat",
        "1280x720_RightMap.dat"};

    DPU_RECT_CHN_ATTR_S astDpuRectChnAttr[DPU_RECT_MAX_CHN_NUM] = {0};
    DPU_MATCH_CHN_ATTR_S stDpuMatchChnAttr = {0};
    HI_U16 u16DispNum = 0;
    HI_U32 u32Size = 0;
    HI_CHAR acThreadName[16] = {0};
    
    /************************************************
     step1:  get all sensors information
     *************************************************/
    SAMPLE_COMM_VI_GetSensorInfo(&s_stSampleViConfig);

    s_stSampleViConfig.s32WorkingViNum = s32ViPipeNum;

    for (i = 0; i < s32ViPipeNum; i++)
    {
        s_stSampleViConfig.astViInfo[i].stDevInfo.enWDRMode        = WDR_MODE_NONE;

        s_stSampleViConfig.astViInfo[i].stChnInfo.enCompressMode   = COMPRESS_MODE_NONE;
        s_stSampleViConfig.astViInfo[i].stChnInfo.enDynamicRange   = DYNAMIC_RANGE_SDR8;
        s_stSampleViConfig.astViInfo[i].stChnInfo.enVideoFormat    = VIDEO_FORMAT_LINEAR;
        s_stSampleViConfig.astViInfo[i].stChnInfo.enPixFormat      = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
        s_stSampleViConfig.astViInfo[i].stChnInfo.ViChn            = 0;

        s_stSampleViConfig.astViInfo[i].stPipeInfo.enMastPipeMode  = VI_OFFLINE_VPSS_OFFLINE;
        s_stSampleViConfig.astViInfo[i].stPipeInfo.aPipe[1]        = -1;
        s_stSampleViConfig.astViInfo[i].stPipeInfo.aPipe[2]        = -1;
        s_stSampleViConfig.astViInfo[i].stPipeInfo.aPipe[3]        = -1;
    }


	switch(left_id){
	case 0:
	    s_stSampleViConfig.as32WorkingViId[0]                   = 0;
	    s_stSampleViConfig.astViInfo[0].stDevInfo.ViDev         = 0;
	    s_stSampleViConfig.astViInfo[0].stPipeInfo.aPipe[0]     = 0;
	    s_stSampleViConfig.astViInfo[0].stSnsInfo.MipiDev       = 0;
	    s_stSampleViConfig.astViInfo[0].stSnsInfo.s32BusId      = 0;
	break;
	case 1:
	    s_stSampleViConfig.as32WorkingViId[0]                   = 0;
	    s_stSampleViConfig.astViInfo[0].stDevInfo.ViDev         = 1;
	    s_stSampleViConfig.astViInfo[0].stPipeInfo.aPipe[0]     = 0;
	    s_stSampleViConfig.astViInfo[0].stSnsInfo.MipiDev       = 1;
	    s_stSampleViConfig.astViInfo[0].stSnsInfo.s32BusId      = 1;
	break;
	case 2:
	    s_stSampleViConfig.as32WorkingViId[0]                   = 0;
	    s_stSampleViConfig.astViInfo[0].stDevInfo.ViDev         = 4;
	    s_stSampleViConfig.astViInfo[0].stPipeInfo.aPipe[0]     = 0;
	    s_stSampleViConfig.astViInfo[0].stSnsInfo.MipiDev       = 4;
	    s_stSampleViConfig.astViInfo[0].stSnsInfo.s32BusId      = 2;
	break;
	case 3:
	    s_stSampleViConfig.as32WorkingViId[0]                   = 0;
	    s_stSampleViConfig.astViInfo[0].stDevInfo.ViDev         = 6;
	    s_stSampleViConfig.astViInfo[0].stPipeInfo.aPipe[0]     = 0;
	    s_stSampleViConfig.astViInfo[0].stSnsInfo.MipiDev       = 6;
	    s_stSampleViConfig.astViInfo[0].stSnsInfo.s32BusId      = 3;
	break;
	}

	switch(right_id){
	case 0:
		s_stSampleViConfig.as32WorkingViId[1]					= 1;
		s_stSampleViConfig.astViInfo[1].stDevInfo.ViDev 		= 0;
		s_stSampleViConfig.astViInfo[1].stPipeInfo.aPipe[0] 	= 1;
		s_stSampleViConfig.astViInfo[1].stSnsInfo.MipiDev		= 0;
		s_stSampleViConfig.astViInfo[1].stSnsInfo.s32BusId		= 0;

	break;
	case 1:
		s_stSampleViConfig.as32WorkingViId[1]					= 1;
		s_stSampleViConfig.astViInfo[1].stDevInfo.ViDev 		= 1;
		s_stSampleViConfig.astViInfo[1].stPipeInfo.aPipe[0] 	= 1;
		s_stSampleViConfig.astViInfo[1].stSnsInfo.MipiDev		= 1;
		s_stSampleViConfig.astViInfo[1].stSnsInfo.s32BusId		= 1;

	break;
	case 2:
		s_stSampleViConfig.as32WorkingViId[1]					= 1;
		s_stSampleViConfig.astViInfo[1].stDevInfo.ViDev 		= 4;
		s_stSampleViConfig.astViInfo[1].stPipeInfo.aPipe[0] 	= 1;
		s_stSampleViConfig.astViInfo[1].stSnsInfo.MipiDev		= 4;
		s_stSampleViConfig.astViInfo[1].stSnsInfo.s32BusId		= 2;

	break;
	case 3:
		s_stSampleViConfig.as32WorkingViId[1]					= 1;
		s_stSampleViConfig.astViInfo[1].stDevInfo.ViDev 		= 6;
		s_stSampleViConfig.astViInfo[1].stPipeInfo.aPipe[0] 	= 1;
		s_stSampleViConfig.astViInfo[1].stSnsInfo.MipiDev		= 6;
		s_stSampleViConfig.astViInfo[1].stSnsInfo.s32BusId		= 3;

	break;
	}

    /************************************************
     step 2: get input size
     *************************************************/
    s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(s_stSampleViConfig.astViInfo[0].stSnsInfo.enSnsType, &enPicSize);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_GetSizeBySensor failed!\n");
        return s32Ret;
    }

    s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed!\n");
        return s32Ret;
    }

	if(need_sysinit) {
	    /******************************************
	      step 3: mpp system init
	     ******************************************/
	    memset(&stVbConfig, 0, sizeof(VB_CONFIG_S));
	    stVbConfig.u32MaxPoolCnt             = 128*2;
	    stVbConfig.astCommPool[0].u32BlkCnt  = 10*2;
	    stVbConfig.astCommPool[0].u64BlkSize = 2*COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height,
	                PIXEL_FORMAT_YVU_SEMIPLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
	    stVbConfig.astCommPool[1].u32BlkCnt  = 8*2;
	    stVbConfig.astCommPool[1].u64BlkSize = 2*COMMON_GetPicBufferSize(SAMPLE_DPU_DST_WIDTH_VI, SAMPLE_DPU_DST_HEIGHT_VI,
	                PIXEL_FORMAT_S16C1, DATA_BITWIDTH_16, COMPRESS_MODE_NONE, DEFAULT_ALIGN);

	    stVbConfig.astCommPool[2].u32BlkCnt  = u32ViChnCnt * 4*2;
	    stVbConfig.astCommPool[2].u64BlkSize = 2*VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height,
	                PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, DEFAULT_ALIGN);

	    stVbConfig.astCommPool[3].u32BlkCnt  = 10*2;
	    stVbConfig.astCommPool[3].u64BlkSize = 2*COMMON_GetPicBufferSize(SAMPLE_DPU_IN_WIDTH, SAMPLE_DPU_IN_HEIGHT, 
	                PIXEL_FORMAT_YVU_SEMIPLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
	    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConfig);
		printf("u32MaxPoolCnt %d \n",stVbConfig.u32MaxPoolCnt);
		for(i = 0;i<4;i++){
			printf("stVbConfig.astCommPool[%d]: u32BlkCnt %d,u64BlkSize %d \n",i,stVbConfig.astCommPool[i].u32BlkCnt,
				stVbConfig.astCommPool[i].u64BlkSize);
		}
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("system init failed with %#x!\n", s32Ret);
	        goto END1;
	    }
	}
    s32Ret = SAMPLE_COMM_VI_SetParam(&s_stSampleViConfig);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SetParam failed with %#x!\n", s32Ret);
        goto END1;
    }
    /******************************************
      step 4: start VI
     ******************************************/
    s32Ret = SAMPLE_COMM_VI_StartVi(&s_stSampleViConfig);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("StartVi failed with %#x\n", s32Ret);
        goto END1;
    }
	
    /******************************************
      step  5: start VPSS
     ******************************************/
    stVpssGrpAttr.u32MaxW                          = 1920;
    stVpssGrpAttr.u32MaxH                          = 1080;
    stVpssGrpAttr.enDynamicRange                   = DYNAMIC_RANGE_SDR8;
    stVpssGrpAttr.enPixelFormat                    = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    stVpssGrpAttr.stFrameRate.s32SrcFrameRate      = -1;
    stVpssGrpAttr.stFrameRate.s32DstFrameRate      = -1;

    astVpssChnAttr[0].u32Width                     = SAMPLE_DPU_IN_WIDTH;
    astVpssChnAttr[0].u32Height                    = SAMPLE_DPU_IN_HEIGHT;
    astVpssChnAttr[0].enChnMode                    = VPSS_CHN_MODE_USER;
    astVpssChnAttr[0].enCompressMode               = COMPRESS_MODE_NONE;
    astVpssChnAttr[0].enDynamicRange               = DYNAMIC_RANGE_SDR8;
    astVpssChnAttr[0].enVideoFormat                = VIDEO_FORMAT_LINEAR;
    astVpssChnAttr[0].enPixelFormat                = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    astVpssChnAttr[0].stFrameRate.s32SrcFrameRate  = -1;
    astVpssChnAttr[0].stFrameRate.s32DstFrameRate  = -1;
    astVpssChnAttr[0].u32Depth                     = 1;
    astVpssChnAttr[0].bMirror                      = HI_FALSE;
    astVpssChnAttr[0].bFlip                        = HI_FALSE;
    astVpssChnAttr[0].stAspectRatio.enMode         = ASPECT_RATIO_NONE;
    astVpssChnAttr[0].stAspectRatio.u32BgColor     = 0xff;

    for (i = 0; i < s32VpssGrpNum; i++)
    {
        VpssGrp = i;
        s32Ret = SAMPLE_COMM_VPSS_Start(VpssGrp, abChnEn, &stVpssGrpAttr, astVpssChnAttr);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("start VPSS fail for %#x!\n", s32Ret);
            goto END2;
        }
        s32VpssGrpNumTmp++;
    }

    /************************************************
     step6:  start DPU RECT
     *************************************************/
    for (i = 0; i < DPU_RECT_MAX_PIPE_NUM; i++)
    {
        s32Ret = SAMPLE_COMM_DPU_RECT_LoadLut(apcMapFileName[i], &s_stSampleDpuConfig.astDpuRectMemInfo[i],
                    &s_stSampleDpuConfig.s32LutId[i]);
        if(s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("load lut failed for %#x!\n", s32Ret);
            goto END3;
        }
        s32DpuRectPipeNum++;
    }

    s_stSampleDpuConfig.stDpuRectGrpAttr.LeftLutId = s_stSampleDpuConfig.s32LutId[0];
    s_stSampleDpuConfig.stDpuRectGrpAttr.RightLutId = s_stSampleDpuConfig.s32LutId[1];
    s_stSampleDpuConfig.DpuRectGrp = 0;
    s_stSampleDpuConfig.stDpuRectGrpAttr.stLeftImageSize.u32Width = SAMPLE_DPU_IN_WIDTH;
    s_stSampleDpuConfig.stDpuRectGrpAttr.stLeftImageSize.u32Height = SAMPLE_DPU_IN_HEIGHT;
    s_stSampleDpuConfig.stDpuRectGrpAttr.stRightImageSize.u32Width = SAMPLE_DPU_IN_WIDTH;
    s_stSampleDpuConfig.stDpuRectGrpAttr.stRightImageSize.u32Height = SAMPLE_DPU_IN_HEIGHT;
    s_stSampleDpuConfig.stDpuRectGrpAttr.enRectMode = DPU_RECT_MODE_DOUBLE;
    s_stSampleDpuConfig.stDpuRectGrpAttr.u32Depth = 1;
    s_stSampleDpuConfig.stDpuRectGrpAttr.bNeedSrcFrame = HI_TRUE;
    s_stSampleDpuConfig.stDpuRectGrpAttr.stFrameRate.s32SrcFrameRate = -1;
    s_stSampleDpuConfig.stDpuRectGrpAttr.stFrameRate.s32DstFrameRate = -1;

    for (i = 0; i < DPU_RECT_MAX_CHN_NUM; i++)
    {
        astDpuRectChnAttr[i].stImageSize.u32Width = SAMPLE_DPU_DST_WIDTH_VI;
        astDpuRectChnAttr[i].stImageSize.u32Height = SAMPLE_DPU_DST_HEIGHT_VI;
    }

    s32Ret = SAMPLE_COMM_DPU_RECT_Start(s_stSampleDpuConfig.DpuRectGrp, 
            &s_stSampleDpuConfig.stDpuRectGrpAttr, astDpuRectChnAttr);
    if(s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("start dpu rect failed for %#x!\n", s32Ret);
        goto END4;
    }

    /************************************************
     step7:  start DPU MATCH
     *************************************************/
    u16DispNum = SAMPLE_DPU_DISP_NUM;
    s32Ret = SAMPLE_COMM_DPU_MATCH_GetAssistBufSize(u16DispNum, SAMPLE_DPU_DST_HEIGHT_VI, &u32Size);
    if(s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("get dpu match assist buffer size failed for %#x!\n", s32Ret);
        goto END4;
    }

    s32Ret = SAMPLE_COMM_DPU_MATCH_CreateMemInfo(&s_stSampleDpuConfig.stDpuMatchGrpAttr.stAssistBuf, u32Size);
    if(s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("create dpu match assist buffer failed for %#x!\n", s32Ret);
        goto END4;
    }

    s_stSampleDpuConfig.DpuMatchGrp = 0;
    s_stSampleDpuConfig.stDpuMatchGrpAttr.u16DispNum = u16DispNum;
    s_stSampleDpuConfig.stDpuMatchGrpAttr.u16DispStartPos = 0;
    s_stSampleDpuConfig.stDpuMatchGrpAttr.u32Depth = 8;
    s_stSampleDpuConfig.stDpuMatchGrpAttr.stLeftImageSize.u32Width = SAMPLE_DPU_DST_WIDTH_VI;
    s_stSampleDpuConfig.stDpuMatchGrpAttr.stLeftImageSize.u32Height = SAMPLE_DPU_DST_HEIGHT_VI;
    s_stSampleDpuConfig.stDpuMatchGrpAttr.stRightImageSize.u32Width = SAMPLE_DPU_DST_WIDTH_VI;
    s_stSampleDpuConfig.stDpuMatchGrpAttr.stRightImageSize.u32Height = SAMPLE_DPU_DST_HEIGHT_VI;
    s_stSampleDpuConfig.stDpuMatchGrpAttr.enDensAccuMode = DPU_MATCH_DENS_ACCU_MODE_D9_A0;
    s_stSampleDpuConfig.stDpuMatchGrpAttr.enDispSubpixelEn = DPU_MATCH_DISP_SUBPIXEL_ENABLE;
    s_stSampleDpuConfig.stDpuMatchGrpAttr.enMatchMaskMode = DPU_MATCH_MASK_9X9_MODE;
    s_stSampleDpuConfig.stDpuMatchGrpAttr.enSpeedAccuMode = DPU_MATCH_SPEED_ACCU_MODE_ACCU;
    s_stSampleDpuConfig.stDpuMatchGrpAttr.bNeedSrcFrame = HI_TRUE;
    s_stSampleDpuConfig.stDpuMatchGrpAttr.stFrameRate.s32SrcFrameRate = -1;
    s_stSampleDpuConfig.stDpuMatchGrpAttr.stFrameRate.s32DstFrameRate = -1;

    stDpuMatchChnAttr.stImageSize.u32Width = SAMPLE_DPU_DST_WIDTH_VI;
    stDpuMatchChnAttr.stImageSize.u32Height = SAMPLE_DPU_DST_HEIGHT_VI;
    s32Ret = SAMPLE_COMM_DPU_MATCH_Start(s_stSampleDpuConfig.DpuMatchGrp, 
                &s_stSampleDpuConfig.stDpuMatchGrpAttr, &stDpuMatchChnAttr);
    if(s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("dpu match start failed for %#x!\n", s32Ret);
        goto END5;
    }

    /************************************************
     step8:  bind VI->VPSS->DPU RECT->DPU MATCH
     *************************************************/
    s32Ret = SAMPLE_DPU_Bind(s_stSampleDpuConfig.DpuRectGrp, s_stSampleDpuConfig.DpuMatchGrp, s32ViPipeNum);
    if(s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("dpu bind failed for %#x!\n", s32Ret);
        goto END6;
    }

	printf("dpu init ok !\n");
	return s32Ret;
	
	END6:
		(HI_VOID)SAMPLE_DPU_UnBind(s_stSampleDpuConfig.DpuRectGrp, s_stSampleDpuConfig.DpuMatchGrp, s32ViPipeNum);
	END5:
		(HI_VOID)SAMPLE_COMM_DPU_MACTH_Stop(s_stSampleDpuConfig.DpuMatchGrp);
		SAMPLE_DPU_MMZ_FREE(s_stSampleDpuConfig.stDpuMatchGrpAttr.stAssistBuf.u64PhyAddr, 
				s_stSampleDpuConfig.stDpuMatchGrpAttr.stAssistBuf.u64VirAddr);
	END4:
		(HI_VOID)SAMPLE_COMM_DPU_RECT_Stop(s_stSampleDpuConfig.DpuRectGrp, 
				s_stSampleDpuConfig.stDpuRectGrpAttr.enRectMode);
	END3:
		for (i = 0; i < s32DpuRectPipeNum; i++)
		{
			(HI_VOID)SAMPLE_COMM_DPU_RECT_UnloadLut(&s_stSampleDpuConfig.astDpuRectMemInfo[i], &s_stSampleDpuConfig.s32LutId[i]);
		}
	END2:
		for (i = 0; i < s32VpssGrpNumTmp; i++)
		{
			VpssGrp = i;
			SAMPLE_COMM_VPSS_Stop(VpssGrp, abChnEn);
		}
		
		SAMPLE_COMM_VI_StopVi(&s_stSampleViConfig);
	END1:
		if(need_sysinit)
			SAMPLE_COMM_SYS_Exit();
    
	printf("dpu init failed !\n");
    return s32Ret;
}

HI_VOID dpu_stop(int need_sysinit)
{
    HI_S32 i = 0;
    HI_S32 s32ViPipeNum = 2;
    HI_S32 s32VpssGrpNum = 2;
    VPSS_GRP VpssGrp = 0;
    HI_BOOL abChnEn[VPSS_MAX_PHY_CHN_NUM] = {1, 0, 0, 0};

    (HI_VOID)SAMPLE_DPU_UnBind(s_stSampleDpuConfig.DpuRectGrp, s_stSampleDpuConfig.DpuMatchGrp, s32ViPipeNum);

    (HI_VOID)SAMPLE_COMM_DPU_MACTH_Stop(s_stSampleDpuConfig.DpuMatchGrp);

    SAMPLE_DPU_MMZ_FREE(s_stSampleDpuConfig.stDpuMatchGrpAttr.stAssistBuf.u64PhyAddr, 
            s_stSampleDpuConfig.stDpuMatchGrpAttr.stAssistBuf.u64VirAddr);

    (HI_VOID)SAMPLE_COMM_DPU_RECT_Stop(s_stSampleDpuConfig.DpuRectGrp, 
            s_stSampleDpuConfig.stDpuRectGrpAttr.enRectMode);

    for (i = 0; i < DPU_RECT_MAX_PIPE_NUM; i++)
    {
        (HI_VOID)SAMPLE_COMM_DPU_RECT_UnloadLut(&s_stSampleDpuConfig.astDpuRectMemInfo[i], &s_stSampleDpuConfig.s32LutId[i]);
    }

    for (i = 0; i < s32VpssGrpNum; i++)
    {
        VpssGrp = i;
        SAMPLE_COMM_VPSS_Stop(VpssGrp, abChnEn);
    }
   
    SAMPLE_COMM_VI_StopVi(&s_stSampleViConfig);
	if(need_sysinit)
    	SAMPLE_COMM_SYS_Exit();

    return ;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
