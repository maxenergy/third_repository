#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

//this file is for ipc,not use fb

#include <sys/poll.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/prctl.h>

#include "sample_comm.h"

#include "hi_math.h"

static int cam_flip = 0; 
static int cam_route = 0;

//#define CAM_WDR_MODE

static SAMPLE_VI_CONFIG_S    stViConfig;

static HI_S32 start_vi(HI_U32 *busid,HI_U32 *devid)
{
    HI_S32                s32Ret = HI_SUCCESS;

    HI_S32                s32ViCnt       = 1;
#ifdef CAM_WDR_MODE	
    VI_PIPE               ViPipe[4]         = {0,1,2,3};
#else
	VI_PIPE 			  ViPipe[4] 		= {0,-1,2,-1};
#endif
    VI_CHN			 ViChn			= 0;

    VI_ROTATION_EX_ATTR_S stViRotationExAttr;

    SIZE_S                stSize;
    VB_CONFIG_S           stVbConf;
    PIC_SIZE_E            enPicSize;
    HI_U32                u32BlkSize;

    VO_CHN                VoChn          = 0;
#ifdef CAM_WDR_MODE
    WDR_MODE_E            enWDRMode      = WDR_MODE_2To1_LINE;//WDR_MODE_NONE;
#else
	WDR_MODE_E			  enWDRMode 	 = WDR_MODE_NONE;//WDR_MODE_NONE;
#endif
    DYNAMIC_RANGE_E       enDynamicRange = DYNAMIC_RANGE_SDR8;
    PIXEL_FORMAT_E        enPixFormat    = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    VIDEO_FORMAT_E        enVideoFormat  = VIDEO_FORMAT_LINEAR;
    COMPRESS_MODE_E       enCompressMode = COMPRESS_MODE_NONE;
    VI_VPSS_MODE_E        enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
	VI_CHN_ATTR_S        vi_pstChnAttr;
    ISP_CTRL_PARAM_S    stIspCtrlParam;

	VPSS_GRP		   VpssGrp		  = 0;
	VPSS_GRP_ATTR_S	  stVpssGrpAttr;
	VPSS_CHN 		  VpssChn		 = VPSS_CHN0;
	HI_BOOL			  abChnEnable[VPSS_MAX_PHY_CHN_NUM] = {0};
	VPSS_CHN_ATTR_S	  astVpssChnAttr[VPSS_MAX_PHY_CHN_NUM];

	

	int i = 0;
    /*config vi*/
    SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);
	stViConfig.s32WorkingViNum									 = s32ViCnt;
    for(i=0;i<s32ViCnt;i++){
	    stViConfig.as32WorkingViId[i]                                = i;
	    stViConfig.astViInfo[i].stSnsInfo.MipiDev         = devid[i];
	    stViConfig.astViInfo[i].stSnsInfo.s32BusId        = busid[i];
	    stViConfig.astViInfo[i].stDevInfo.ViDev           = devid[i];
	    stViConfig.astViInfo[i].stDevInfo.enWDRMode       = enWDRMode;
	    stViConfig.astViInfo[i].stPipeInfo.enMastPipeMode = enMastPipeMode;
	    stViConfig.astViInfo[i].stPipeInfo.aPipe[0]       = ViPipe[i*2];
	    stViConfig.astViInfo[i].stPipeInfo.aPipe[1]       = ViPipe[i*2+1];
	    stViConfig.astViInfo[i].stPipeInfo.aPipe[2]       = -1;
	    stViConfig.astViInfo[i].stPipeInfo.aPipe[3]       = -1;
	    stViConfig.astViInfo[i].stChnInfo.ViChn           = ViChn;
	    stViConfig.astViInfo[i].stChnInfo.enPixFormat     = enPixFormat;
	    stViConfig.astViInfo[i].stChnInfo.enDynamicRange  = enDynamicRange;
	    stViConfig.astViInfo[i].stChnInfo.enVideoFormat   = enVideoFormat;
	    stViConfig.astViInfo[i].stChnInfo.enCompressMode  = enCompressMode;
    }
#if 0
    s32Ret = HI_MPI_ISP_GetCtrlParam(stViConfig.astViInfo[0].stPipeInfo.aPipe[0], &stIspCtrlParam);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_ISP_GetCtrlParam failed with %d!\n", s32Ret);
        return s32Ret;
    }
    stIspCtrlParam.u32StatIntvl  = 30/30;

    s32Ret = HI_MPI_ISP_SetCtrlParam(stViConfig.astViInfo[0].stPipeInfo.aPipe[0], &stIspCtrlParam);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_ISP_SetCtrlParam failed with %d!\n", s32Ret);
        return s32Ret;
    }
#endif
    /*start vi*/
    s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("ver 2 start vi failed.s32Ret:0x%x !\n", s32Ret);
        return s32Ret;
    }
#if 1
	for(i=0;i<s32ViCnt;i++){
	    s32Ret = HI_MPI_VI_SetChnRotation(ViPipe[i*2], ViChn, cam_route);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("HI_MPI_VI_SetChnRotation failed with %d\n", s32Ret);
	       return s32Ret;
	    }

		HI_MPI_VI_GetChnAttr(ViPipe[i*2], ViChn, &vi_pstChnAttr);
		vi_pstChnAttr.u32Depth = 3;
		if(cam_flip)
			vi_pstChnAttr.bFlip = HI_TRUE;
		else
			vi_pstChnAttr.bFlip = HI_FALSE;
		HI_MPI_VI_SetChnAttr(ViPipe[i*2],ViChn,&vi_pstChnAttr);
	}
#endif
	//SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);

	/*config vpss*/
	   hi_memset(&stVpssGrpAttr, sizeof(VPSS_GRP_ATTR_S), 0, sizeof(VPSS_GRP_ATTR_S));
	   stVpssGrpAttr.stFrameRate.s32SrcFrameRate	= -1;
	   stVpssGrpAttr.stFrameRate.s32DstFrameRate	= -1;
	   stVpssGrpAttr.enDynamicRange 				= DYNAMIC_RANGE_SDR8;
	   stVpssGrpAttr.enPixelFormat					= enPixFormat;
	   stVpssGrpAttr.u32MaxW						= 1920;
	   stVpssGrpAttr.u32MaxH						= 1080;
	   stVpssGrpAttr.bNrEn							= HI_TRUE;
	   stVpssGrpAttr.stNrAttr.enCompressMode		= COMPRESS_MODE_FRAME;
	   stVpssGrpAttr.stNrAttr.enNrMotionMode		= NR_MOTION_MODE_NORMAL;
	
	   astVpssChnAttr[VpssChn].u32Width 				   = 1920;
	   astVpssChnAttr[VpssChn].u32Height				   = 1080;
	   astVpssChnAttr[VpssChn].enChnMode				   = VPSS_CHN_MODE_USER;
	   astVpssChnAttr[VpssChn].enCompressMode			   = enCompressMode;
	   astVpssChnAttr[VpssChn].enDynamicRange			   = enDynamicRange;
	   astVpssChnAttr[VpssChn].enVideoFormat			   = enVideoFormat;
	   astVpssChnAttr[VpssChn].enPixelFormat			   = enPixFormat;
	   astVpssChnAttr[VpssChn].stFrameRate.s32SrcFrameRate = 30;
	   astVpssChnAttr[VpssChn].stFrameRate.s32DstFrameRate = 30;
	   astVpssChnAttr[VpssChn].u32Depth 				   = 3;
	   astVpssChnAttr[VpssChn].bMirror					   = HI_FALSE;
	   astVpssChnAttr[VpssChn].bFlip					   = HI_FALSE;
	   astVpssChnAttr[VpssChn].stAspectRatio.enMode 	   = ASPECT_RATIO_NONE;

		abChnEnable[0] = HI_TRUE;
		s32Ret = SAMPLE_COMM_VPSS_Start(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
		if (HI_SUCCESS != s32Ret)
		{
		    SAMPLE_PRT("start vpss group failed. s32Ret: 0x%x !\n", s32Ret);
		    return -1;
		}

		/*vi bind vpss*/
		s32Ret = SAMPLE_COMM_VI_Bind_VPSS(ViPipe[0], ViChn, VpssGrp);
		if (HI_SUCCESS != s32Ret)
		{
		    SAMPLE_PRT("vi bind vpss failed. s32Ret: 0x%x !\n", s32Ret);
		    return -1;
		}
	
    return s32Ret;
}

static HI_S32 start_venc(HI_U32 chnid)
{
    HI_S32 s32Ret = HI_SUCCESS;

    VI_ROTATION_EX_ATTR_S stViRotationExAttr;
	VENC_GOP_ATTR_S stGopAttr;

    /* config venc */
	PIC_SIZE_E enSize;
	PAYLOAD_TYPE_E enType = PT_H264;
	SAMPLE_RC_E enRcMode = SAMPLE_RC_VBR;
	HI_U32  u32Profile = 0;
	HI_BOOL bRcnRefShareBuf = HI_TRUE;
	stGopAttr.enGopMode = VENC_GOPMODE_NORMALP;
	stGopAttr.stNormalP.s32IPQpDelta = 2;
    SAMPLE_PRT("stGopAttr.stNormalP.s32IPQpDelta[%d]\n", stGopAttr.stNormalP.s32IPQpDelta);

	SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[0].stSnsInfo.enSnsType, &enSize);

    /*start venc*/
    s32Ret = SAMPLE_COMM_VENC_Start(chnid, enType,  enSize, enRcMode, u32Profile, bRcnRefShareBuf, &stGopAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VENC_Start[%d] failed.s32Ret:0x%x !\n", chnid, s32Ret);
        return s32Ret;
    }
	VENC_CHN VeChn[1] = { chnid };
	HI_S32 s32Cnt = 1;
	SAMPLE_VENC_PROCFRAME_MODE_E mode = VENC_PROCFRAME_TO_CALLBACK;

	s32Ret = SAMPLE_COMM_VENC_StartGetStream(VeChn, s32Cnt, mode);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VENC_Start[%d] failed.s32Ret:0x%x !\n", chnid, s32Ret);
        return s32Ret;
    }

    return s32Ret;
}


int sys_init()
{
    HI_S32                   s32Ret        = HI_SUCCESS;
    VB_CONFIG_S              stVbConf;
    HI_U32			   u32BlkSize;
    /******************************************
     step  1: init variable
    ******************************************/
	
    memset(&stVbConf, 0, sizeof(VB_CONFIG_S));

    stVbConf.u32MaxPoolCnt              = 2;

    u32BlkSize = COMMON_GetPicBufferSize(1920, 1080, SAMPLE_PIXEL_FORMAT, DATA_BITWIDTH_8, COMPRESS_MODE_SEG, DEFAULT_ALIGN);
    stVbConf.astCommPool[0].u64BlkSize  = u32BlkSize;
    stVbConf.astCommPool[0].u32BlkCnt   = 20;

    u32BlkSize = VI_GetRawBufferSize(1920, 1080, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
    stVbConf.astCommPool[1].u64BlkSize  = u32BlkSize;
    stVbConf.astCommPool[1].u32BlkCnt   = 20;
    /******************************************
     step 2: mpp system init.
    ******************************************/
    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("system init failed with %d!\n", s32Ret);
		return -1;
    }

    return s32Ret;
}
#ifdef BUILD_SDK2V0
extern int scence_param_create(char* Path);
#endif
static void cam_turing()
{
	ISP_NR_ATTR_S ispattr;
	 HI_MPI_ISP_GetNRAttr(0,&ispattr);
	 ispattr.enOpType = OP_TYPE_MANUAL;
	 ispattr.stManual.au8ChromaStr[0] = 0x3;
	 ispattr.stManual.au8ChromaStr[1] = 0x3;
	 ispattr.stManual.au8ChromaStr[2] = 0x3;
	 ispattr.stManual.au8ChromaStr[3] = 0x3;
	 ispattr.stManual.u8FineStr = 0x80;
	 ispattr.stManual.u16CoringWgt = 10;
	 ispattr.stManual.au16CoarseStr[0] = 0x360;
	 ispattr.stManual.au16CoarseStr[1] = 0x360;
	 ispattr.stManual.au16CoarseStr[2] = 0x360;
	 ispattr.stManual.au16CoarseStr[3] = 0x360;

	 HI_MPI_ISP_SetNRAttr(0,&ispattr);

}
int sysinit(int *fd_out,int mroute,int mflip)
{
    HI_S32           s32Ret       = HI_FAILURE;
	HI_U32 busid[2] = {0,1};
	HI_U32 devid[2] = {0,1};

	switch(mroute){
	case 0:
		cam_route = ROTATION_0;
		break;
	case 90:
		cam_route = ROTATION_90;
		break;
	case 180:
		cam_route = ROTATION_180;
		break;
	case 270:
		cam_route = ROTATION_270;
		break;
	default:
		cam_route = ROTATION_0;
		break;
	}
	cam_flip  = mflip;
    sys_init();
	start_vi(busid,devid);
	#ifdef BUILD_SDK2V0
	scence_param_create("/home/param");
	#else	
	cam_turing();
    #endif
    s32Ret = start_venc(SAMPLE_VENC_CHNID);
	SAMPLE_COMM_VPSS_Bind_VENC(0,
		0, SAMPLE_VENC_CHNID);

    return (s32Ret);
}

int pwm_led_state=0;

struct pwm_data{
	int pwm_num;
	int duty;
	int period;
	int enable;
};

HI_S32  PWM_Write(HI_U8 nPwmIdx, HI_U32 nDuty, HI_U32 nPeriod, HI_BOOL bEnable)
{
  int ret = 0;
  struct pwm_data stPwmData;
  HI_S32 s32Ret;
  HI_S32 fd;
  
  if(pwm_led_state == bEnable)
  	return;
  
  fd = open("/dev/pwm", 2);
  if ( fd >= 0 )
  {
    stPwmData.pwm_num = nPwmIdx;
    stPwmData.duty = nDuty;
    stPwmData.period = nPeriod;
    stPwmData.enable = bEnable;
    s32Ret = ioctl(fd, 1u, &stPwmData);
    if ( s32Ret )
    {
      printf("write pwm dev failed \n");
    }
	pwm_led_state = bEnable;
  }
  else
  {
    printf("open pwm dev failed \n");
    ret = -1;
  }
  
  close(fd);
  return ret;
}

int SetIRLedLight(unsigned int light)
{
  if (light > 0xFF )
  {
		light = 0xFF;
  }

  PWM_Write(0, light, 0xFFu, (HI_BOOL)(light != 0));
  return 0;
}

void sysexit(void)
{
	SAMPLE_COMM_VI_UnBind_VENC(stViConfig.astViInfo[0].stPipeInfo.aPipe[0],
		stViConfig.astViInfo[0].stChnInfo.ViChn, SAMPLE_VENC_CHNID);
	
	SAMPLE_COMM_VENC_StopGetStream();
    SAMPLE_COMM_VENC_Stop(SAMPLE_VENC_CHNID);
	SAMPLE_COMM_VENC_UnRegCallback(SAMPLE_VENC_CHNID);
    SAMPLE_COMM_VI_StopVi(&stViConfig);
    SAMPLE_COMM_SYS_Exit();
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

