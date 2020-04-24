
#include "sample_comm_dpu.h"
#include "mpi_dpu_rect.h"
#include "mpi_dpu_match.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/******************************************************************************
* function : calculate stride
******************************************************************************/
HI_U32 SAMPLE_COMM_DPU_CalcStride(HI_U32 u32Width, HI_U8 u8Align)
{
    return (u32Width + (u8Align - u32Width%u8Align)%u8Align);
}

/******************************************************************************
* function : create dpu rect memory info
******************************************************************************/
HI_S32 SAMPLE_COMM_DPU_RECT_CreateMemInfo(DPU_RECT_MEM_INFO_S *pstMemInfo,HI_U32 u32Size)
{
    HI_S32 s32Ret = HI_SUCCESS;

    pstMemInfo->u32Size = u32Size;
    s32Ret = HI_MPI_SYS_MmzAlloc(&pstMemInfo->u64PhyAddr, (void**)&pstMemInfo->u64VirAddr, NULL, NULL, u32Size);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("HI_MPI_SYS_MmzAlloc failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    return s32Ret;
}

/*****************************************************************************
* function : load dpu rect lut.
*****************************************************************************/
HI_S32 SAMPLE_COMM_DPU_RECT_LoadLut(HI_CHAR *pszFileName, 
    DPU_RECT_MEM_INFO_S *pstMemInfo, DPU_RECT_LUT_ID *ps32LutId)
{
    HI_S32 s32Ret = HI_FAILURE;
    HI_U32 u32Size;
    
    s32Ret = HI_MPI_DPU_RECT_GetLutSize(pszFileName, &u32Size);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("HI_MPI_DPU_RECT_GetLutSize failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    memset(pstMemInfo, 0, sizeof(*pstMemInfo));
    s32Ret = SAMPLE_COMM_DPU_RECT_CreateMemInfo(pstMemInfo, u32Size);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("DPU_RECT_MST_CreateMemInfo failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_DPU_RECT_LoadLut(pszFileName, pstMemInfo, ps32LutId);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("HI_MPI_DPU_RECT_LoadLut failed with %#x!\n", s32Ret);
        goto FAIL;
    }
    
    return s32Ret;

FAIL:
    SAMPLE_DPU_MMZ_FREE(pstMemInfo->u64PhyAddr, pstMemInfo->u64VirAddr);

    return s32Ret;
}

/*****************************************************************************
* function : unload dpu rect lut.
*****************************************************************************/
HI_VOID SAMPLE_COMM_DPU_RECT_UnloadLut(DPU_RECT_MEM_INFO_S *pstMemInfo,DPU_RECT_LUT_ID *ps32LutId)
{
    (HI_VOID)HI_MPI_DPU_RECT_UnloadLut(*ps32LutId);
    SAMPLE_DPU_MMZ_FREE(pstMemInfo->u64PhyAddr, pstMemInfo->u64VirAddr);
}

/*****************************************************************************
* function : start dpu rect grp.
*****************************************************************************/
HI_S32 SAMPLE_COMM_DPU_RECT_Start(DPU_RECT_GRP DpuRectGrp,
    DPU_RECT_GRP_ATTR_S *pstGrpAttr, DPU_RECT_CHN_ATTR_S* pastChnAttr)
{
    DPU_RECT_CHN DpuRectChn;
    HI_S32 s32Ret;
    HI_S32 j;
    HI_U32 DpuRectChnNum = 0;

    s32Ret = HI_MPI_DPU_RECT_CreateGrp(DpuRectGrp, pstGrpAttr);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("HI_MPI_DPU_RECT_CreateGrp(grp:%d) failed with %#x!\n", DpuRectGrp, s32Ret);
        return HI_FAILURE;
    }

    if (DPU_RECT_MODE_SINGLE == pstGrpAttr->enRectMode)
    {
        DpuRectChnNum = 1;
    }
    else
    {
        DpuRectChnNum = DPU_RECT_MAX_CHN_NUM;
    }
        
    for (j = 0; j < DpuRectChnNum; j++)
    {
        DpuRectChn = j;
        s32Ret = HI_MPI_DPU_RECT_SetChnAttr(DpuRectGrp, DpuRectChn, &pastChnAttr[DpuRectChn]);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("HI_MPI_DPU_RECT_SetChnAttr failed with %#x\n", s32Ret);
            return HI_FAILURE;
        }

        s32Ret = HI_MPI_DPU_RECT_EnableChn(DpuRectGrp, DpuRectChn);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("HI_MPI_DPU_RECT_EnableChn failed with %#x\n", s32Ret);
            return HI_FAILURE;
        }
    }

    s32Ret = HI_MPI_DPU_RECT_StartGrp(DpuRectGrp);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("HI_MPI_DPU_RECT_StartGrp failed with %#x\n", s32Ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*****************************************************************************
* function : stop dpu rect grp
*****************************************************************************/
HI_S32 SAMPLE_COMM_DPU_RECT_Stop(DPU_RECT_GRP DpuRectGrp, DPU_RECT_MODE_E enRectMode)
{
    HI_S32 j;
    HI_S32 s32Ret = HI_SUCCESS;
    DPU_RECT_CHN DpuRectChn;
    HI_U32 DpuRectChnNum = 0;

    s32Ret = HI_MPI_DPU_RECT_StopGrp(DpuRectGrp);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    if (DPU_RECT_MODE_SINGLE == enRectMode)
    {
        DpuRectChnNum = 1;
    }
    else
    {
        DpuRectChnNum = DPU_RECT_MAX_CHN_NUM;
    }
        
    for (j = 0; j < DpuRectChnNum; j++)
    {
        DpuRectChn = j;
        s32Ret = HI_MPI_DPU_RECT_DisableChn(DpuRectGrp, DpuRectChn);

        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("failed with %#x!\n", s32Ret);
            return HI_FAILURE;
        }
    }

    s32Ret = HI_MPI_DPU_RECT_DestroyGrp(DpuRectGrp);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*****************************************************************************
* function : read frame from file
*****************************************************************************/
HI_S32 SAMPLE_COMM_DPU_RECT_ReadOneFrame(FILE * pFp, HI_U8 *pu8Y,
    HI_U32 u32Width, HI_U32 u32Height, HI_U32 u32Stride)
{
    HI_U8 *pu8Dst = NULL;
    HI_U32 u32Row = 0;
    HI_S32 s32Ret;
    
    pu8Dst = pu8Y;
    for (u32Row = 0; u32Row < u32Height; u32Row++)
    {
        s32Ret = fread( pu8Dst, u32Width, 1, pFp);
        if (s32Ret != 1)
        {
            SAMPLE_PRT("failed with %#x!\n", s32Ret);
            return HI_FAILURE;
        }
        pu8Dst += u32Stride;
    }

    return HI_SUCCESS;
}

/*****************************************************************************
* function : get frame from file
*****************************************************************************/
HI_S32 SAMPLE_COMM_DPU_RECT_GetVFrameFromFile(FILE *pFp, VB_POOL hPool, HI_U32 u32Width,
    HI_U32 u32Height,HI_U32 u32Stride, VIDEO_FRAME_INFO_S *pstVFrameInfo, VB_BLK VbBlk)
{
    HI_U32 u32Size;
    HI_U64 u64PhyAddr;
    HI_U64 *pu64VirAddr;
    HI_S32 s32Ret;

    u32Size = u32Stride * u32Height;
    u64PhyAddr = HI_MPI_VB_Handle2PhysAddr(VbBlk);
    if (0 == u64PhyAddr)
    {
        SAMPLE_PRT("HI_MPI_VB_Handle2PhysAddr failed!\n");
        goto FAIL_0;
    }

    pu64VirAddr = (HI_U64 *)HI_MPI_SYS_Mmap(u64PhyAddr, u32Size);
    if (NULL == pu64VirAddr)
    {
        SAMPLE_PRT("HI_MPI_SYS_Mmap failed!\n");
        goto FAIL_0;
    }

    pstVFrameInfo->u32PoolId = HI_MPI_VB_Handle2PoolId(VbBlk);
    if (VB_INVALID_POOLID == pstVFrameInfo->u32PoolId)
    {
        SAMPLE_PRT("HI_MPI_VB_Handle2PoolId failed!\n");
        goto FAIL_1;
    }

    pstVFrameInfo->stVFrame.u64PhyAddr[0] = u64PhyAddr;
    pstVFrameInfo->stVFrame.u64VirAddr[0] = (HI_U64)(HI_UL)pu64VirAddr;
    pstVFrameInfo->stVFrame.u32Width  = u32Width;
    pstVFrameInfo->stVFrame.u32Height = u32Height;
    pstVFrameInfo->stVFrame.u32Stride[0] = u32Stride;
    pstVFrameInfo->stVFrame.enCompressMode = COMPRESS_MODE_NONE;
    pstVFrameInfo->stVFrame.enVideoFormat = VIDEO_FORMAT_LINEAR;
    pstVFrameInfo->stVFrame.enDynamicRange = DYNAMIC_RANGE_SDR8;
    pstVFrameInfo->stVFrame.enPixelFormat = PIXEL_FORMAT_YUV_400;
    pstVFrameInfo->stVFrame.enField = VIDEO_FIELD_FRAME;

    s32Ret = SAMPLE_COMM_DPU_RECT_ReadOneFrame(pFp, (HI_U8*)(HI_UL)pstVFrameInfo->stVFrame.u64VirAddr[0],
        pstVFrameInfo->stVFrame.u32Width, pstVFrameInfo->stVFrame.u32Height, pstVFrameInfo->stVFrame.u32Stride[0]);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_DPU_RECT_ReadOneFrame failed with %#x!\n", s32Ret);
        goto FAIL_1;
    }

    (HI_VOID)HI_MPI_SYS_Munmap(pu64VirAddr, u32Size);
    pstVFrameInfo->stVFrame.u64VirAddr[0] = 0;
    pu64VirAddr = NULL;
    
    return HI_SUCCESS;
FAIL_1:
    (HI_VOID)HI_MPI_SYS_Munmap(pu64VirAddr, u32Size);
FAIL_0:

    return HI_FAILURE;
}


void deepth_TO_RGB24(HI_U16 *data, unsigned char *rgb, int width, int height,HI_U32 u32Stride) {
	int x =0;
	int y =0;
	int index =0;
	for ( y = 0; y < height; y++) {
		for ( x = 0; x < width; x++) {
			float deepth = data[x + y*u32Stride];
				  deepth = deepth/255;

		    if(deepth > 255)
				deepth = 255;
			else if(deepth < 0)
				deepth = 0;
			
			rgb[index++] = (unsigned char)deepth;
			rgb[index++] = 0;
			rgb[index++] = 0;
		}
	}
}


/******************************************************************************
* function : write  image to binary file
******************************************************************************/
static HI_S32 SAMPLE_COMM_DPU_Write2BinaryFile(FILE *pFp, HI_U8 *pu8Src, HI_U32 u32Stride,
    HI_U32 u32Width, HI_U32 u32Height, HI_U32 u32EleSize)
{
	HI_U32 i = 0;
    HI_S32 s32Ret = HI_SUCCESS;
	unsigned char rgb[u32Width*u32Height*3];
	deepth_TO_RGB24((HI_U16*)pu8Src,rgb,u32Width,u32Height,u32Stride/2);
	HI_U8 *rgb_p =(HI_U8 *)rgb;

	for(i=0; i<u32Height; i++)
	{
        s32Ret = fwrite(rgb_p, u32Width * 3, 1, pFp);
        if (1 != s32Ret)
        {
            SAMPLE_PRT("fwrite file failed with %#x!\n", s32Ret);
            return HI_FAILURE;
        }
		rgb_p += u32Width*3;
	}

    fflush(pFp);

	return  HI_SUCCESS;
}


static HI_S32 SAMPLE_COMM_DPU_Write2buf_depth(void *depth_buf, HI_U8 *pu8Src, HI_U32 u32Stride,
	HI_U32 u32Width, HI_U32 u32Height, HI_U32 u32EleSize)
{
	deepth_TO_RGB24((HI_U16*)pu8Src,(unsigned char*)depth_buf,u32Width,u32Height,u32Stride/2);
	return	HI_SUCCESS;
}


	void YVU420P_TO_RGB24(unsigned char *data, unsigned char *rgb, int width, int height,HI_U32 u32Stride) {
		int index = 0;
		unsigned char *ybase = data;
		unsigned char *uvbase = &data[width * height];
        unsigned char *ubase = &data[width * height+width * height/4];
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
			//YYYYVU
				u_char Y = ybase[x + y * u32Stride];
				u_char V = uvbase[y / 2 * u32Stride + (x / 2)*2];
				u_char U = uvbase[y / 2 * u32Stride + (x / 2)*2+1];
				int r = Y + 1.402 * (V - 128);
				if(r >255)
					r = 255;
				else if(r <0)
					r =0;

				int g = Y - 0.34413 * (U - 128) - 0.71414 * (V - 128); //G
				if(g >255)
					g = 255;
				else if(g <0)
					g =0;

				
				int b = Y + 1.772 * (U - 128); //B
				if(b >255)
					b = 255;
				else if(b <0)
					b =0;
				
				rgb[index++] = r;
				
				rgb[index++] = g; //G
				rgb[index++] = b; //B
			}
		}
	}


	static HI_S32 SAMPLE_COMM_DPU_Write2BinaryFile_yuv420p(FILE *pFp, HI_U8 *pu8Src, HI_U32 u32Stride,
		HI_U32 u32Width, HI_U32 u32Height, HI_U32 u32EleSize)
	{
		HI_U32 i = 0;
		HI_S32 s32Ret = HI_SUCCESS;
		unsigned char rgb[u32Width*u32Height*3];
		YVU420P_TO_RGB24(pu8Src,rgb,u32Width,u32Height,u32Stride);
		HI_U8 *rgb_p =(HI_U8 *)rgb;
		for(i=0; i<u32Height;i++)
		{
			s32Ret = fwrite(rgb_p, u32Width * u32EleSize*3,1, pFp);
			if (1!= s32Ret)
			{
				SAMPLE_PRT("fwrite file failed with %#x!\n", s32Ret);
				return HI_FAILURE;
			}
			rgb_p += u32Width*3;
		}
	
		fflush(pFp);
	
		return	HI_SUCCESS;
	}


static HI_S32 SAMPLE_COMM_DPU_Write2buf_yuv420p(void* rgb_buf, HI_U8 *pu8Src, HI_U32 u32Stride,
	HI_U32 u32Width, HI_U32 u32Height, HI_U32 u32EleSize)
{
	YVU420P_TO_RGB24(pu8Src,(unsigned char*)rgb_buf,u32Width,u32Height,u32Stride);
	return	HI_SUCCESS;
}

/*****************************************************************************
* function : write frame to file.
*****************************************************************************/
HI_S32 SAMPLE_COMM_DPU_WriteFrame2buf_yuv420p(void *buf, HI_U32 u32EleSize, VIDEO_FRAME_INFO_S *pstVFrameInfo)
{
	HI_U32 u32Stride = 0;
	HI_U32 u32Width = 0;
	HI_U32 u32Height = 0;
	HI_U8 *pu8VirAddr = NULL;
	HI_U32 u32Size = 0;
	HI_S32 s32Ret= HI_SUCCESS;
	
	u32Width = pstVFrameInfo->stVFrame.u32Width;
	u32Height = pstVFrameInfo->stVFrame.u32Height;
	u32Stride = pstVFrameInfo->stVFrame.u32Stride[0];
	u32Size = u32Stride * u32Height + u32Stride * u32Height/2;
	pu8VirAddr = (HI_U8 *)HI_MPI_SYS_Mmap(pstVFrameInfo->stVFrame.u64PhyAddr[0], u32Size);
	if (NULL == pu8VirAddr)
	{
		SAMPLE_PRT("HI_MPI_SYS_Mmap failed!\n");
		return HI_FAILURE;
	}

	s32Ret = SAMPLE_COMM_DPU_Write2buf_yuv420p(buf, pu8VirAddr, u32Stride, u32Width, u32Height, u32EleSize);

	HI_MPI_SYS_Munmap(pu8VirAddr, u32Size);
	pu8VirAddr = NULL;

	return s32Ret;
}



/*****************************************************************************
* function : write frame to file.
*****************************************************************************/
HI_S32 SAMPLE_COMM_DPU_WriteFrame2File_yuv420p(FILE *pFp, HI_U32 u32EleSize, VIDEO_FRAME_INFO_S *pstVFrameInfo)
{
	HI_U32 u32Stride = 0;
	HI_U32 u32Width = 0;
	HI_U32 u32Height = 0;
	HI_U8 *pu8VirAddr = NULL;
	HI_U32 u32Size = 0;
	HI_S32 s32Ret= HI_SUCCESS;
	
	u32Width = pstVFrameInfo->stVFrame.u32Width;
	u32Height = pstVFrameInfo->stVFrame.u32Height;
	u32Stride = pstVFrameInfo->stVFrame.u32Stride[0];
	u32Size = u32Stride * u32Height + u32Stride * u32Height/2;
	printf("u32Width %u u32Height %u u32Stride %u u32Size %u \n",u32Width,u32Height,u32Stride,u32Size);

	pu8VirAddr = (HI_U8 *)HI_MPI_SYS_Mmap(pstVFrameInfo->stVFrame.u64PhyAddr[0], u32Size);
	if (NULL == pu8VirAddr)
	{
		SAMPLE_PRT("HI_MPI_SYS_Mmap failed!\n");
		return HI_FAILURE;
	}

	s32Ret = SAMPLE_COMM_DPU_Write2BinaryFile_yuv420p(pFp, pu8VirAddr, u32Stride, u32Width, u32Height, u32EleSize);

	HI_MPI_SYS_Munmap(pu8VirAddr, u32Size);
	pu8VirAddr = NULL;

	return s32Ret;
}


/*****************************************************************************
* function : write frame to file.
*****************************************************************************/
HI_S32 SAMPLE_COMM_DPU_WriteFrame2buf_depth(void *buf, HI_U32 u32EleSize, VIDEO_FRAME_INFO_S *pstVFrameInfo)
{
    HI_U32 u32Stride = 0;
    HI_U32 u32Width = 0;
    HI_U32 u32Height = 0;
    HI_U8 *pu8VirAddr = NULL;
    HI_U32 u32Size = 0;
    HI_S32 s32Ret= HI_SUCCESS;
    
    u32Width = pstVFrameInfo->stVFrame.u32Width;
    u32Height = pstVFrameInfo->stVFrame.u32Height;
    u32Stride = pstVFrameInfo->stVFrame.u32Stride[0];
    u32Size = u32Stride * u32Height;
	printf("u32Width %u u32Height %u u32Stride %u u32Size %u \n",u32Width,u32Height,u32Stride,u32Size);

    pu8VirAddr = (HI_U8 *)HI_MPI_SYS_Mmap(pstVFrameInfo->stVFrame.u64PhyAddr[0], u32Size);
	if (NULL == pu8VirAddr)
	{
        SAMPLE_PRT("HI_MPI_SYS_Mmap failed!\n");
        return HI_FAILURE;
    }

    s32Ret = SAMPLE_COMM_DPU_Write2buf_depth(buf, pu8VirAddr, u32Stride, u32Width, u32Height, u32EleSize);

    HI_MPI_SYS_Munmap(pu8VirAddr, u32Size);
    pu8VirAddr = NULL;

    return s32Ret;
}


/*****************************************************************************
* function : write frame to file.
*****************************************************************************/
HI_S32 SAMPLE_COMM_DPU_WriteFrame2File(FILE *pFp, HI_U32 u32EleSize, VIDEO_FRAME_INFO_S *pstVFrameInfo)
{
    HI_U32 u32Stride = 0;
    HI_U32 u32Width = 0;
    HI_U32 u32Height = 0;
    HI_U8 *pu8VirAddr = NULL;
    HI_U32 u32Size = 0;
    HI_S32 s32Ret= HI_SUCCESS;
    
    u32Width = pstVFrameInfo->stVFrame.u32Width;
    u32Height = pstVFrameInfo->stVFrame.u32Height;
    u32Stride = pstVFrameInfo->stVFrame.u32Stride[0];
    u32Size = u32Stride * u32Height;
	printf("u32Width %u u32Height %u u32Stride %u u32Size %u \n",u32Width,u32Height,u32Stride,u32Size);

    pu8VirAddr = (HI_U8 *)HI_MPI_SYS_Mmap(pstVFrameInfo->stVFrame.u64PhyAddr[0], u32Size);
	if (NULL == pu8VirAddr)
	{
        SAMPLE_PRT("HI_MPI_SYS_Mmap failed!\n");
        return HI_FAILURE;
    }

    s32Ret = SAMPLE_COMM_DPU_Write2BinaryFile(pFp, pu8VirAddr, u32Stride, u32Width, u32Height, u32EleSize);

    HI_MPI_SYS_Munmap(pu8VirAddr, u32Size);
    pu8VirAddr = NULL;

    return s32Ret;
}

/*****************************************************************************
* function : get dpu match assist buffer size.
*****************************************************************************/
HI_S32 SAMPLE_COMM_DPU_MATCH_GetAssistBufSize(HI_U16 u16DispNum, HI_U32 u32DstHeight, HI_U32 *pu32Size)
{
    return HI_MPI_DPU_MATCH_GetAssistBufSize(u16DispNum, u32DstHeight, pu32Size);
}

/******************************************************************************
* function : create dpu match memory info
******************************************************************************/
HI_S32 SAMPLE_COMM_DPU_MATCH_CreateMemInfo(DPU_MATCH_MEM_INFO_S *pstMemInfo,HI_U32 u32Size)
{
    HI_S32 s32Ret = HI_SUCCESS;

    pstMemInfo->u32Size = u32Size;
    s32Ret = HI_MPI_SYS_MmzAlloc(&pstMemInfo->u64PhyAddr, (void**)&pstMemInfo->u64VirAddr, NULL, NULL, u32Size);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("HI_MPI_SYS_MmzAlloc failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    return s32Ret;
}

/*****************************************************************************
* function : start dpu match grp.
*****************************************************************************/
HI_S32 SAMPLE_COMM_DPU_MATCH_Start(DPU_MATCH_GRP DpuMatchGrp,
    DPU_MATCH_GRP_ATTR_S *pstGrpAttr, DPU_MATCH_CHN_ATTR_S* pstChnAttr)
{
    DPU_MATCH_CHN DpuMatchChn;
    HI_S32 s32Ret;
    HI_S32 j;

    s32Ret = HI_MPI_DPU_MATCH_CreateGrp(DpuMatchGrp, pstGrpAttr);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("HI_MPI_DPU_RECT_CreateGrp(grp:%d) failed with %#x!\n", DpuMatchGrp, s32Ret);
        return HI_FAILURE;
    }

    for (j = 0; j < DPU_MATCH_MAX_CHN_NUM; j++)
    {
        DpuMatchChn = j;
        s32Ret = HI_MPI_DPU_MATCH_SetChnAttr(DpuMatchGrp, DpuMatchChn, pstChnAttr);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("HI_MPI_DPU_MATCH_SetChnAttr failed with %#x\n", s32Ret);
            return HI_FAILURE;
        }

        s32Ret = HI_MPI_DPU_MATCH_EnableChn(DpuMatchGrp, DpuMatchChn);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("HI_MPI_DPU_MATCH_EnableChn failed with %#x\n", s32Ret);
            return HI_FAILURE;
        }
    }

    s32Ret = HI_MPI_DPU_MATCH_StartGrp(DpuMatchGrp);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("HI_MPI_DPU_MATCH_StartGrp failed with %#x\n", s32Ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*****************************************************************************
* function : stop dpu match grp
*****************************************************************************/
HI_S32 SAMPLE_COMM_DPU_MACTH_Stop(DPU_MATCH_GRP DpuMatchGrp)
{
    HI_S32 j;
    HI_S32 s32Ret = HI_SUCCESS;
    DPU_MATCH_CHN DpuMatchChn;

    s32Ret = HI_MPI_DPU_MATCH_StopGrp(DpuMatchGrp);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    for (j = 0; j < DPU_MATCH_MAX_CHN_NUM; j++)
    {
        DpuMatchChn = j;
        s32Ret = HI_MPI_DPU_MATCH_DisableChn(DpuMatchGrp, DpuMatchChn);

        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("failed with %#x!\n", s32Ret);
            return HI_FAILURE;
        }
    }

    s32Ret = HI_MPI_DPU_MATCH_DestroyGrp(DpuMatchGrp);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*****************************************************************************
* function : bind dpu rect to match
*****************************************************************************/
HI_S32 SAMPLE_COMM_DPU_RECT_Bind_MATCH(DPU_RECT_GRP DpuRectGrp, DPU_MATCH_GRP DpuMatchGrp)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = HI_ID_DPU_RECT;
    stSrcChn.s32DevId  = DpuRectGrp;
    stSrcChn.s32ChnId  = 0;

    stDestChn.enModId  = HI_ID_DPU_MATCH;
    stDestChn.s32DevId = DpuMatchGrp;
    stDestChn.s32ChnId = 0;

    CHECK_RET(HI_MPI_SYS_Bind(&stSrcChn, &stDestChn), "HI_MPI_SYS_Bind(RECT-MATCH)");

    return HI_SUCCESS;
}

/*****************************************************************************
* function : unbind dpu rect to match
*****************************************************************************/
HI_S32 SAMPLE_COMM_DPU_RECT_UnBind_MATCH(DPU_RECT_GRP DpuRectGrp, DPU_MATCH_GRP DpuMatchGrp)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId  = HI_ID_DPU_RECT;
    stSrcChn.s32DevId = DpuRectGrp;
    stSrcChn.s32ChnId = 0;

    stDestChn.enModId  = HI_ID_DPU_MATCH;
    stDestChn.s32DevId = DpuMatchGrp;
    stDestChn.s32ChnId = 0;

    CHECK_RET(HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "HI_MPI_SYS_UnBind(RECT-MATCH)");

    return HI_SUCCESS;
}

/*****************************************************************************
* function : bind vpss to dpu rect
*****************************************************************************/
HI_S32 SAMPLE_COMM_DPU_VPSS_Bind_RECT(VPSS_GRP VpssGrp, VPSS_CHN VpssChn,
        DPU_RECT_GRP DpuRectGrp, DPU_RECT_PIPE DpuRectPipe)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = HI_ID_VPSS;
    stSrcChn.s32DevId  = VpssGrp;
    stSrcChn.s32ChnId  = VpssChn;

    stDestChn.enModId  = HI_ID_DPU_RECT;
    stDestChn.s32DevId = DpuRectGrp;
    stDestChn.s32ChnId = DpuRectPipe;

    CHECK_RET(HI_MPI_SYS_Bind(&stSrcChn, &stDestChn), "HI_MPI_SYS_Bind(VPSS-RECT)");

    return HI_SUCCESS;
}

/*****************************************************************************
* function : unbind vpss to dpu rect
*****************************************************************************/
HI_S32 SAMPLE_COMM_DPU_VPSS_UnBind_RECT(VPSS_GRP VpssGrp, VPSS_CHN VpssChn,
        DPU_RECT_GRP DpuRectGrp, DPU_RECT_PIPE DpuRectPipe)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId  = HI_ID_VPSS;
    stSrcChn.s32DevId = VpssGrp;
    stSrcChn.s32ChnId = VpssChn;

    stDestChn.enModId  = HI_ID_DPU_RECT;
    stDestChn.s32DevId = DpuRectGrp;
    stDestChn.s32ChnId = DpuRectGrp;

    CHECK_RET(HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "HI_MPI_SYS_UnBind(VPSS-RECT)");

    return HI_SUCCESS;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
