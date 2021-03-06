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

#include <linux/fb.h>
#include "hifb.h"
#include "hi_tde_api.h"
#include "hi_tde_type.h"
#include "hi_tde_errcode.h"
#include "hi_math.h"
static SAMPLE_VI_CONFIG_S    stViConfig;
static SAMPLE_VO_CONFIG_S    stVoConfig;
static struct fb_bitfield s_a32 = {24, 8, 0};
static struct fb_bitfield s_r32 = {16, 8, 0};
static struct fb_bitfield s_g32 = {8,  8, 0};
static struct fb_bitfield s_b32 = {0,  8, 0};
static HI_S32 start_mpp(HI_U32 *busid,HI_U32 *devid)
{
    HI_S32                s32Ret = HI_SUCCESS;

    HI_S32                s32ViCnt       = 2;
    VI_PIPE               ViPipe[2]         = {0,2};
    VI_CHN			 ViChn			= 0;

    VI_ROTATION_EX_ATTR_S stViRotationExAttr;

    SIZE_S                stSize;
    VB_CONFIG_S           stVbConf;
    PIC_SIZE_E            enPicSize;
    HI_U32                u32BlkSize;

    VO_CHN                VoChn          = 0;

    WDR_MODE_E            enWDRMode      = WDR_MODE_NONE;
    DYNAMIC_RANGE_E       enDynamicRange = DYNAMIC_RANGE_SDR8;
    PIXEL_FORMAT_E        enPixFormat    = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    VIDEO_FORMAT_E        enVideoFormat  = VIDEO_FORMAT_LINEAR;
    COMPRESS_MODE_E       enCompressMode = COMPRESS_MODE_NONE;
    VI_VPSS_MODE_E        enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
	VI_CHN_ATTR_S        vi_pstChnAttr;
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
	    stViConfig.astViInfo[i].stPipeInfo.aPipe[0]       = ViPipe[i];
	    stViConfig.astViInfo[i].stPipeInfo.aPipe[1]       = -1;
	    stViConfig.astViInfo[i].stPipeInfo.aPipe[2]       = -1;
	    stViConfig.astViInfo[i].stPipeInfo.aPipe[3]       = -1;
	    stViConfig.astViInfo[i].stChnInfo.ViChn           = ViChn;
	    stViConfig.astViInfo[i].stChnInfo.enPixFormat     = enPixFormat;
	    stViConfig.astViInfo[i].stChnInfo.enDynamicRange  = enDynamicRange;
	    stViConfig.astViInfo[i].stChnInfo.enVideoFormat   = enVideoFormat;
	    stViConfig.astViInfo[i].stChnInfo.enCompressMode  = enCompressMode;
    }
    /*start vi*/
    s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("ver 2 start vi failed.s32Ret:0x%x !\n", s32Ret);
        return s32Ret;
    }

    /*vi bind vo*/
    s32Ret = SAMPLE_COMM_VI_Bind_VO(ViPipe[0], ViChn, 0, VoChn);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("vo bind vpss failed. s32Ret: 0x%x !\n", s32Ret);
        return s32Ret;
    }


	for(i=0;i<s32ViCnt;i++){
	    s32Ret = HI_MPI_VI_SetChnRotation(ViPipe[i], ViChn, ROTATION_90);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("HI_MPI_VI_SetChnRotation failed with %d\n", s32Ret);
	       return s32Ret;
	    }

		HI_MPI_VI_GetChnAttr(ViPipe[i], ViChn, &vi_pstChnAttr);
		vi_pstChnAttr.u32Depth = 8;
		vi_pstChnAttr.bFlip = HI_TRUE;
		HI_MPI_VI_SetChnAttr(ViPipe[i],ViChn,&vi_pstChnAttr);
	}
    return s32Ret;
}

int fb_start(int *fd_out)
{
		struct fb_fix_screeninfo fix;
		struct fb_var_screeninfo var;
		HIFB_POINT_S			 stPoint			= {0, 0};
		HI_BOOL 				 bShow;
		HI_U32					 u32Width;
		HI_U32					 u32Height;
		/********************************
		* Step 1. open framebuffer device overlay 0
		**********************************/
		*fd_out = open("/dev/fb0", O_RDWR, 0);
		if (*fd_out < 0)
		{
            SAMPLE_PRT("open /dev/fb0 failed!\n");
			return HI_NULL;
		}
	
		bShow = HI_FALSE;
		if (ioctl(*fd_out, FBIOPUT_SHOW_HIFB, &bShow) < 0)
		{
			SAMPLE_PRT("FBIOPUT_SHOW_HIFB failed!\n");
			return HI_NULL;
		}
		/********************************
		* Step 2. set the screen original position
		**********************************/
		stPoint.s32XPos = 0;
		stPoint.s32YPos = 0;	
		if (ioctl(*fd_out, FBIOPUT_SCREEN_ORIGIN_HIFB, &stPoint) < 0)
		{
			SAMPLE_PRT("set screen original show position failed!\n");
            close(*fd_out);
			return HI_NULL;
		}
	
		/********************************
		* Step 3. get the variable screen information
		**********************************/
		if (ioctl(*fd_out, FBIOGET_VSCREENINFO, &var) < 0)
		{
			SAMPLE_PRT("Get variable screen info failed!\n");
			close(*fd_out);
			return HI_NULL;
		}
		var.transp = s_a32;
		var.red    = s_r32;
		var.green  = s_g32;
		var.blue   = s_b32;
		var.bits_per_pixel = 32;
		var.xres_virtual = 800;
		var.yres_virtual = 1280 * 2;
		var.xres		 = 800;
		var.yres		 = 1280;
		var.activate	   = FB_ACTIVATE_NOW;
	
		/*********************************
		* Step 5. set the variable screen information
		***********************************/
		if (ioctl(*fd_out, FBIOPUT_VSCREENINFO, &var) < 0)
		{
			SAMPLE_PRT("Put variable screen info failed!\n");
			close(*fd_out);
			return HI_NULL;
		}
		/**********************************
		* Step 6. get the fix screen information
		************************************/
        if (ioctl(*fd_out, FBIOGET_FSCREENINFO, &fix) < 0)
		{
			SAMPLE_PRT("Get fix screen info failed!\n");
            close(*fd_out);
			return HI_NULL;
		}
		bShow = HI_TRUE;
        if (ioctl(*fd_out, FBIOPUT_SHOW_HIFB, &bShow) < 0)
		{
			SAMPLE_PRT("FBIOPUT_SHOW_HIFB failed!\n");
			return HI_NULL;
		}
}

int set_mode(SAMPLE_VO_CONFIG_S*   pstVoDevInfo)
{
    HI_S32                   s32Ret        = HI_SUCCESS;
    VO_DEV                   VoDev         = pstVoDevInfo->VoDev;
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

    /******************************************
     step 3: Start VO device.
     NOTE: Step 3 is optional when VO is running on other system.
    ******************************************/
    s32Ret = SAMPLE_COMM_VO_StartVO(pstVoDevInfo);

	
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VO_StartVO failed with %d!\n", s32Ret);
		return -1;
    }

    return s32Ret;
}

static  SAMPLE_VO_CONFIG_S      stVoDevInfo;
int sysinit(int *fd_out)
{
    HI_S32           s32Ret       = HI_FAILURE;
    HI_CHAR          ch;

    RECT_S  stDefDispRect  = {0, 0, 800, 1280};
    SIZE_S  stDefImageSize = {800, 1280};
	HI_U32 busid[2] = {0,1};
	HI_U32 devid[2] = {0,1};
    stVoDevInfo.VoDev             = SAMPLE_VO_DEV_UHD;
    stVoDevInfo.enVoIntfType      = VO_INTF_MIPI;
    stVoDevInfo.enIntfSync        = VO_OUTPUT_USER;
    stVoDevInfo.u32BgColor        = COLOR_RGB_BLACK;
    stVoDevInfo.enPixFormat       = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    stVoDevInfo.stDispRect        = stDefDispRect;
    stVoDevInfo.stImageSize       = stDefImageSize;
    stVoDevInfo.enVoPartMode      = VO_PART_MODE_SINGLE;
    stVoDevInfo.u32DisBufLen      = 3;
    stVoDevInfo.enDstDynamicRange = DYNAMIC_RANGE_SDR8;
    stVoDevInfo.enVoMode          = VO_MODE_1MUX;
    set_mode(&stVoDevInfo);
	start_mpp(busid,devid);
	fb_start(&fd_out);
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
 SAMPLE_COMM_VI_UnBind_VO(0,0,0,0);
  SAMPLE_COMM_VO_StopVO(&stVoDevInfo);
  SAMPLE_COMM_VI_StopVi(&stViConfig);
  SAMPLE_COMM_SYS_Exit();
}
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

