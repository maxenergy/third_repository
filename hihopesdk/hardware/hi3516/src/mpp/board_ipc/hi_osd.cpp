extern "C" {
#include "sample_comm.h"
}
#include <opencv2/opencv.hpp>
#include <sys/time.h>

void osd_update_text(char* pic_buf,int w,int h,char * ip_title,int fsize)
{
	cv::Mat cv_image = cv::Mat::zeros(cv::Size(w, h), CV_8UC3);
	char time_title[100];
	int i = 0;
	struct tm *t;
	time_t tt;
	time(&tt);
	t = localtime(&tt);
	sprintf(time_title,"%4d/%02d/%02d %02d:%02d:%02d",t->tm_year + 1900,t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
	CvScalar draw_color = cv::Scalar(255,255,255);
	cv::putText(cv_image, cv::String(time_title), cvPoint(w/10,25*fsize), cv::FONT_HERSHEY_COMPLEX, fsize, draw_color, fsize, 8, 0);
	cv::putText(cv_image, cv::String(ip_title), cvPoint(w/10,60*fsize), cv::FONT_HERSHEY_COMPLEX, fsize, draw_color, fsize, 8, 0);
	cv::Mat dstImage;
	cv::cvtColor(cv_image, dstImage, cv::COLOR_RGB2BGR555);
	for(i=0;i<w*h;i++)
	{
		int pix = dstImage.data[i*2]+dstImage.data[i*2+1];
		if(pix == 0)
			dstImage.data[i*2+1] = dstImage.data[i*2+1]|0x80;
	}
	memcpy(pic_buf,dstImage.data,w*h*2);
}

void update_osd(unsigned int handle,char* bitmap_buf, int w,int h,char * ip_title)
{
	BITMAP_S		stBitmap;
	int s32Ret;

	if(bitmap_buf == NULL){
		bitmap_buf = (char*) malloc(w*h*2);
	}
	memset(bitmap_buf,0,w*h*2);
	stBitmap.u32Width		= w;
	stBitmap.u32Height		= h;
	stBitmap.enPixelFormat	= PIXEL_FORMAT_ARGB_1555;
	stBitmap.pData          = bitmap_buf;
	if(handle == 0){
		osd_update_text((char*)bitmap_buf,w,h,ip_title,2);
	}else{
		osd_update_text((char*)bitmap_buf,w,h,ip_title,1);
	}
	s32Ret = HI_MPI_RGN_SetBitMap(handle, &stBitmap);
	if(HI_SUCCESS != s32Ret)
    {
        printf("%s: HI_MPI_RGN_SetBitMap (%d) failed with %#x!\n", __FUNCTION__, handle, s32Ret);
        return ;
    }
}

 int creat_venc_osd(int channel_id,int x,int y,int w,int h,unsigned int handle)
{
	RGN_ATTR_S stRgnAttr;
	MPP_CHN_S		stChn;
    RGN_CHN_ATTR_S	stChnAttr;
	BITMAP_S		stBitmap;
	int s32Ret;

	memset(&stRgnAttr, 0, sizeof(RGN_ATTR_S));
	memset(&stChnAttr, 0, sizeof(RGN_CHN_ATTR_S));
	stRgnAttr.enType							= OVERLAY_RGN;
	stRgnAttr.unAttr.stOverlay.enPixelFmt		= PIXEL_FORMAT_ARGB_1555;
	stRgnAttr.unAttr.stOverlay.stSize.u32Width  = w;
	stRgnAttr.unAttr.stOverlay.stSize.u32Height = h;
	stRgnAttr.unAttr.stOverlayEx.u32CanvasNum = 2;

	s32Ret = HI_MPI_RGN_Create(handle, &stRgnAttr);
	if(HI_SUCCESS != s32Ret)
	{
		HI_MPI_RGN_Destroy(handle);
		printf("error! creat RGN FAILED! %d 0x%x\n",handle,s32Ret);
		return 0;
	}

    stChnAttr.bShow  = HI_TRUE;
    stChnAttr.enType = OVERLAY_RGN;
    stChnAttr.unChnAttr.stOverlayChn.stPoint.s32X = x;
    stChnAttr.unChnAttr.stOverlayChn.stPoint.s32Y = y;
    stChnAttr.unChnAttr.stOverlayChn.u32BgAlpha   = 128;
    stChnAttr.unChnAttr.stOverlayChn.u32FgAlpha   = 0;
    stChnAttr.unChnAttr.stOverlayChn.u32Layer     = 0;

    stChnAttr.unChnAttr.stOverlayChn.stQpInfo.bAbsQp = HI_FALSE;
    stChnAttr.unChnAttr.stOverlayChn.stQpInfo.s32Qp  = 0;
    
	stChnAttr.unChnAttr.stOverlayChn.stInvertColor.stInvColArea.u32Height = 16;
	stChnAttr.unChnAttr.stOverlayChn.stInvertColor.stInvColArea.u32Width  = 16;
	stChnAttr.unChnAttr.stOverlayChn.stInvertColor.u32LumThresh           = 0x80;

    stChnAttr.unChnAttr.stOverlayChn.stInvertColor.enChgMod     = MORETHAN_LUM_THRESH;
	stChnAttr.unChnAttr.stOverlayChn.stInvertColor.bInvColEn			  = HI_TRUE;
	
	stChn.enModId  = HI_ID_VENC;
	stChn.s32DevId = 0;
	stChn.s32ChnId = channel_id;

	s32Ret = HI_MPI_RGN_AttachToChn(handle, &stChn, &stChnAttr);
    if(HI_SUCCESS != s32Ret)
    {
        printf("%s: HI_MPI_RGN_AttachToChn (%d) failed with %#x!\n", __FUNCTION__, handle, s32Ret);
        return HI_FAILURE;
    }
	printf("AttachToChn ok!!\n");
	return HI_SUCCESS;
}
