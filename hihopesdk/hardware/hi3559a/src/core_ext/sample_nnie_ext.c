#include "sample_nnie_ext.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/*yolov3-tiny para*/
static SAMPLE_SVP_NNIE_MODEL_S s_stYolov3tinyModel = {0};
static SAMPLE_SVP_NNIE_PARAM_S s_stYolov3tinyNnieParam = {0};
static SAMPLE_SVP_NNIE_YOLOV3_TINY_SOFTWARE_PARAM_S s_stYolov3tinySoftwareParam = {0};

HI_S32 SAMPLE_SVP_NNIE_IPC_FillSrcData(
        SAMPLE_SVP_NNIE_CFG_S* pstNnieCfg,
        SAMPLE_SVP_NNIE_PARAM_S *pstNnieParam,
        SAMPLE_SVP_NNIE_INPUT_DATA_INDEX_S *pstInputDataIdx)
{
    FILE* fp = NULL;
    HI_U32 i =0, j = 0, n = 0;
    HI_U32 u32Height = 0, u32Width = 0, u32Chn = 0, u32Stride = 0, u32Dim = 0;
    HI_U32 u32VarSize = 0;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U8*pu8PicAddr = NULL;
    HI_U32*pu32StepAddr = NULL;
    HI_U32 u32SegIdx = pstInputDataIdx->u32SegIdx;
    HI_U32 u32NodeIdx = pstInputDataIdx->u32NodeIdx;
    HI_U32 u32TotalStepNum = 0;

    /*open file*/
    if (NULL != pstNnieCfg->pszPic)
    {
        fp = fopen(pstNnieCfg->pszPic, "rb");
        SAMPLE_SVP_CHECK_EXPR_RET(
                    NULL == fp,
                    HI_INVALID_VALUE,SAMPLE_SVP_ERR_LEVEL_ERROR,
                    "Error, open file failed!\n");
    }

    /*get data size*/
    if(SVP_BLOB_TYPE_U8 <= pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].enType &&
            SVP_BLOB_TYPE_YVU422SP >= pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].enType)
    {
        u32VarSize = sizeof(HI_U8);
    } else {
        u32VarSize = sizeof(HI_U32);
    }

    printf("ipc fill data %d \n",pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u32Num);

    /*fill src data*/
    if(SVP_BLOB_TYPE_SEQ_S32 == pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].enType)
    {
        u32Dim = pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].unShape.stSeq.u32Dim;
        u32Stride = pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u32Stride;
        pu32StepAddr = (HI_U32*)(pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].unShape.stSeq.u64VirAddrStep);
        pu8PicAddr = (HI_U8*)(pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u64VirAddr);

        for(n = 0; n < pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u32Num; n++)
        {
            for(i = 0;i < *(pu32StepAddr+n); i++)
            {
                s32Ret = fread(pu8PicAddr,u32Dim*u32VarSize,1,fp);
                SAMPLE_SVP_CHECK_EXPR_GOTO(1 != s32Ret,FAIL,SAMPLE_SVP_ERR_LEVEL_ERROR,"Error,Read image file failed!\n");
                pu8PicAddr += u32Stride;
                printf("SVP_BLOB_TYPE_SEQ_S32 read count i=%d \n",i);
            }
            u32TotalStepNum += *(pu32StepAddr+n);
        }
        SAMPLE_COMM_SVP_FlushCache(
                    pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u64PhyAddr,
                    (HI_VOID *) pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u64VirAddr,
                    u32TotalStepNum*u32Stride);
    } else {
        u32Height = pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].unShape.stWhc.u32Height;
        u32Width = pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].unShape.stWhc.u32Width;
        u32Chn = pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].unShape.stWhc.u32Chn;
        u32Stride = pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u32Stride;
        pu8PicAddr = (HI_U8*)(pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u64VirAddr);
        if(SVP_BLOB_TYPE_YVU420SP== pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].enType)
        {
            for(n = 0; n < pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u32Num; n++)
            {
                for(i = 0; i < u32Chn*u32Height/2; i++)
                {
                    s32Ret = fread(pu8PicAddr, u32Width*u32VarSize, 1, fp);
                    SAMPLE_SVP_CHECK_EXPR_GOTO(1 != s32Ret,FAIL,SAMPLE_SVP_ERR_LEVEL_ERROR,"Error,Read image file failed!\n");
                    pu8PicAddr += u32Stride;

                    printf("SVP_BLOB_TYPE_YVU420SP read count i=%d \n",i);
                }
            }
        } else if(SVP_BLOB_TYPE_YVU422SP == pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].enType) {
            for(n = 0; n < pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u32Num; n++)
            {
                for(i = 0; i < u32Height*2; i++)
                {
                    s32Ret = fread(pu8PicAddr,u32Width*u32VarSize,1,fp);
                    SAMPLE_SVP_CHECK_EXPR_GOTO(1 != s32Ret,FAIL,SAMPLE_SVP_ERR_LEVEL_ERROR,"Error,Read image file failed!\n");
                    pu8PicAddr += u32Stride;

                    printf("SVP_BLOB_TYPE_YVU422SP read count i=%d \n",i);
                }
            }
        } else {
            for(n = 0; n < pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u32Num; n++)
            {
                for(i = 0;i < u32Chn; i++)
                {
                    for(j = 0; j < u32Height; j++)
                    {
                        s32Ret = fread(pu8PicAddr, u32Width*u32VarSize, 1, fp);
                        SAMPLE_SVP_CHECK_EXPR_GOTO(1 != s32Ret,FAIL,SAMPLE_SVP_ERR_LEVEL_ERROR,"Error,Read image file failed!\n");
                        pu8PicAddr += u32Stride;

                        printf("else read count i=%d \n",i);
                    }
                }
            }
        }
        SAMPLE_COMM_SVP_FlushCache(
                    pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u64PhyAddr,
                    (HI_VOID *) pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u64VirAddr,
                    pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u32Num*u32Chn*u32Height*u32Stride);
    }

    fclose(fp);
    return HI_SUCCESS;
FAIL:

    fclose(fp);
    return HI_FAILURE;
}

static HI_S32 read_data_from_ptr(HI_U8 * dst, int size, int count, void *src)
{
    memcpy(dst, src, size*count);
    return count;
}

static HI_S32 SAMPLE_SVP_NNIE_FillSrcData_ptr(
        void *buf,
        SAMPLE_SVP_NNIE_PARAM_S *pstNnieParam,
        SAMPLE_SVP_NNIE_INPUT_DATA_INDEX_S *pstInputDataIdx)
{
    HI_U8 *data = (HI_U8 *)buf;
    HI_U32 i =0, j = 0, n = 0;
    HI_U32 u32Height = 0, u32Width = 0, u32Chn = 0, u32Stride = 0, u32Dim = 0;
    HI_U32 u32VarSize = 0;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U8*pu8PicAddr = NULL;
    HI_U32*pu32StepAddr = NULL;
    HI_U32 u32SegIdx = pstInputDataIdx->u32SegIdx;
    HI_U32 u32NodeIdx = pstInputDataIdx->u32NodeIdx;
    HI_U32 u32TotalStepNum = 0;

    /*get data size*/
    if(SVP_BLOB_TYPE_U8 <= pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].enType &&
            SVP_BLOB_TYPE_YVU422SP >= pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].enType)
    {
        u32VarSize = sizeof(HI_U8);
    } else {
        u32VarSize = sizeof(HI_U32);
    }

    /*fill src data*/
    if(SVP_BLOB_TYPE_SEQ_S32 == pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].enType)
    {
        u32Dim = pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].unShape.stSeq.u32Dim;
        u32Stride = pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u32Stride;
        pu32StepAddr = (HI_U32*)(pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].unShape.stSeq.u64VirAddrStep);
        pu8PicAddr = (HI_U8*)(pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u64VirAddr);
        for(n = 0; n < pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u32Num; n++)
        {
            for(i = 0;i < *(pu32StepAddr+n); i++)
            {
                s32Ret = read_data_from_ptr(pu8PicAddr, u32Dim*u32VarSize, 1, data);
                data += u32Dim*u32VarSize;
                SAMPLE_SVP_CHECK_EXPR_GOTO(1 != s32Ret,FAIL,SAMPLE_SVP_ERR_LEVEL_ERROR,"Error,Read image file failed!\n");
                pu8PicAddr += u32Stride;
            }
            u32TotalStepNum += *(pu32StepAddr+n);
        }
        SAMPLE_COMM_SVP_FlushCache(
                    pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u64PhyAddr,
                    (HI_VOID *) pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u64VirAddr,
                    u32TotalStepNum*u32Stride);
    } else {
        u32Height = pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].unShape.stWhc.u32Height;
        u32Width = pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].unShape.stWhc.u32Width;
        u32Chn = pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].unShape.stWhc.u32Chn;
        u32Stride = pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u32Stride;
        pu8PicAddr = (HI_U8*)(pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u64VirAddr);
        if(SVP_BLOB_TYPE_YVU420SP== pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].enType)
        {
            for(n = 0; n < pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u32Num; n++)
            {
                for(i = 0; i < u32Chn*u32Height/2; i++)
                {
                    s32Ret = read_data_from_ptr(pu8PicAddr,u32Width*u32VarSize,1,data);
                    data += u32Width*u32VarSize;
                    SAMPLE_SVP_CHECK_EXPR_GOTO(1 != s32Ret,FAIL,SAMPLE_SVP_ERR_LEVEL_ERROR,"Error,Read image file failed!\n");
                    pu8PicAddr += u32Stride;
                }
            }
        } else if (SVP_BLOB_TYPE_YVU422SP== pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].enType) {
            for(n = 0; n < pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u32Num; n++)
            {
                for(i = 0; i < u32Height*2; i++)
                {
                    s32Ret = read_data_from_ptr(pu8PicAddr,u32Width*u32VarSize,1,data);
                    data += u32Width*u32VarSize;
                    SAMPLE_SVP_CHECK_EXPR_GOTO(1 != s32Ret,FAIL,SAMPLE_SVP_ERR_LEVEL_ERROR,"Error,Read image file failed!\n");
                    pu8PicAddr += u32Stride;
                }
            }
        } else {
            for(n = 0; n < pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u32Num; n++)
            {
                for(i = 0;i < u32Chn; i++)
                {
                    for(j = 0; j < u32Height; j++)
                    {
                        s32Ret = read_data_from_ptr(pu8PicAddr, u32Width*u32VarSize, 1, data);
                        data += u32Width*u32VarSize;
                        SAMPLE_SVP_CHECK_EXPR_GOTO(1 != s32Ret, FAIL, SAMPLE_SVP_ERR_LEVEL_ERROR, "Error,Read image file failed!\n");
                        pu8PicAddr += u32Stride;
                    }
                }
            }
        }
        SAMPLE_COMM_SVP_FlushCache(
                    pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u64PhyAddr,
                    (HI_VOID *) pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u64VirAddr,
                    pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u32Num*u32Chn*u32Height*u32Stride);
    }

    return HI_SUCCESS;

FAIL:
    return HI_FAILURE;
}

HI_S32 SAMPLE_SVP_NNIE_Detection_Result(
        SVP_BLOB_S *pstDstScore,
        SVP_BLOB_S *pstDstRoi,
        SVP_BLOB_S *pstClassRoiNum,
        HI_FLOAT f32PrintResultThresh,
        DETECT_RET_BUF *ret_buf)
{
    HI_U32 i = 0, j = 0;
    HI_U32 u32RoiNumBias = 0;
    HI_U32 u32ScoreBias = 0;
    HI_U32 u32BboxBias = 0;
    HI_FLOAT f32Score = 0.0f;
    HI_S32* ps32Score = (HI_S32*)pstDstScore->u64VirAddr;
    HI_S32* ps32Roi = (HI_S32*)pstDstRoi->u64VirAddr;
    HI_S32* ps32ClassRoiNum = (HI_S32*)pstClassRoiNum->u64VirAddr;
    HI_U32 u32ClassNum = pstClassRoiNum->unShape.stWhc.u32Width;
    HI_S32 s32XMin = 0,s32YMin= 0,s32XMax = 0,s32YMax = 0;
    int count = 0;
    int current_class=0;
    ret_buf->ret_count = 0;
    u32RoiNumBias += ps32ClassRoiNum[0];
    for (i = 1; i < u32ClassNum; i++)
    {
        u32ScoreBias = u32RoiNumBias;
        u32BboxBias = u32RoiNumBias * SAMPLE_SVP_NNIE_COORDI_NUM;
        /*if the confidence score greater than result threshold, the result will be printed*/
        if((HI_FLOAT)ps32Score[u32ScoreBias] / SAMPLE_SVP_NNIE_QUANT_BASE >=
                f32PrintResultThresh && ps32ClassRoiNum[i]!=0)
        {
            //  SAMPLE_SVP_TRACE_INFO("==== The %dth class box info====\n", i);
            current_class = i;
        }
        for (j = 0; j < (HI_U32)ps32ClassRoiNum[i]; j++)
        {
            f32Score = (HI_FLOAT)ps32Score[u32ScoreBias + j] / SAMPLE_SVP_NNIE_QUANT_BASE;
            if (f32Score < f32PrintResultThresh)
            {
                break;
            }
            s32XMin = ps32Roi[u32BboxBias + j*SAMPLE_SVP_NNIE_COORDI_NUM];
            s32YMin = ps32Roi[u32BboxBias + j*SAMPLE_SVP_NNIE_COORDI_NUM + 1];
            s32XMax = ps32Roi[u32BboxBias + j*SAMPLE_SVP_NNIE_COORDI_NUM + 2];
            s32YMax = ps32Roi[u32BboxBias + j*SAMPLE_SVP_NNIE_COORDI_NUM + 3];
            ret_buf->class_ret[count] = current_class;
            ret_buf->x0[count] = s32XMin;
            ret_buf->x1[count] = s32XMax;
            ret_buf->y0[count] = s32YMin;
            ret_buf->y1[count] = s32YMax;
            count++;
            ret_buf->ret_count = count;
            if(count >= MAX_RET_COUNT)
                return HI_SUCCESS;
        }
        u32RoiNumBias += ps32ClassRoiNum[i];
    }
    return HI_SUCCESS;
}

static HI_S32 SAMPLE_SVP_NNIE_Yolov3_tiny_SoftwareDeinit(SAMPLE_SVP_NNIE_YOLOV3_TINY_SOFTWARE_PARAM_S* pstSoftWareParam)
{
    HI_S32 s32Ret = HI_SUCCESS;
    SAMPLE_SVP_CHECK_EXPR_RET(NULL== pstSoftWareParam,HI_INVALID_VALUE,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error, pstSoftWareParam can't be NULL!\n");
    if(0!=pstSoftWareParam->stGetResultTmpBuf.u64PhyAddr && 0!=pstSoftWareParam->stGetResultTmpBuf.u64VirAddr)
    {
        SAMPLE_SVP_MMZ_FREE(pstSoftWareParam->stGetResultTmpBuf.u64PhyAddr,
            pstSoftWareParam->stGetResultTmpBuf.u64VirAddr);
        pstSoftWareParam->stGetResultTmpBuf.u64PhyAddr = 0;
        pstSoftWareParam->stGetResultTmpBuf.u64VirAddr = 0;
        pstSoftWareParam->stDstRoi.u64PhyAddr = 0;
        pstSoftWareParam->stDstRoi.u64VirAddr = 0;
        pstSoftWareParam->stDstScore.u64PhyAddr = 0;
        pstSoftWareParam->stDstScore.u64VirAddr = 0;
        pstSoftWareParam->stClassRoiNum.u64PhyAddr = 0;
        pstSoftWareParam->stClassRoiNum.u64VirAddr = 0;
    }
    return s32Ret;
}

static HI_S32 SAMPLE_SVP_NNIE_Yolov3_tiny_SoftwareInit(
        SAMPLE_SVP_NNIE_CFG_S *pstCfg,
        SAMPLE_SVP_NNIE_PARAM_S *pstNnieParam,
        SAMPLE_SVP_NNIE_YOLOV3_TINY_SOFTWARE_PARAM_S *pstSoftWareParam)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32ClassNum = 0;
    HI_U32 u32TotalSize = 0;
    HI_U32 u32DstRoiSize = 0;
    HI_U32 u32DstScoreSize = 0;
    HI_U32 u32ClassRoiNumSize = 0;
    HI_U32 u32TmpBufTotalSize = 0;
    HI_U64 u64PhyAddr = 0;
    HI_U8* pu8VirAddr = NULL;

    pstSoftWareParam->u32OriImHeight = pstNnieParam->astSegData[0].astSrc[0].unShape.stWhc.u32Height;
    pstSoftWareParam->u32OriImWidth = pstNnieParam->astSegData[0].astSrc[0].unShape.stWhc.u32Width;
    pstSoftWareParam->u32BboxNumEachGrid = 3;
    pstSoftWareParam->u32ClassNum = 80;
    pstSoftWareParam->au32GridNumHeight[0] = 13;
    pstSoftWareParam->au32GridNumHeight[1] = 26;
    pstSoftWareParam->au32GridNumWidth[0] = 13;
    pstSoftWareParam->au32GridNumWidth[1] = 26;
    pstSoftWareParam->u32NmsThresh = (HI_U32)(0.3f*SAMPLE_SVP_NNIE_QUANT_BASE);
    pstSoftWareParam->u32ConfThresh = (HI_U32)(0.3f*SAMPLE_SVP_NNIE_QUANT_BASE);
    pstSoftWareParam->u32MaxRoiNum = MAX_RET_COUNT;
    //10,14,  23,27,  37,58,  81,82,  135,169,  344,319
    pstSoftWareParam->af32Bias[0][0] = 81;
    pstSoftWareParam->af32Bias[0][1] = 82;
    pstSoftWareParam->af32Bias[0][2] = 135;
    pstSoftWareParam->af32Bias[0][3] = 169;
    pstSoftWareParam->af32Bias[0][4] = 344;
    pstSoftWareParam->af32Bias[0][5] = 319;

    pstSoftWareParam->af32Bias[1][0] = 10;
    pstSoftWareParam->af32Bias[1][1] = 14;
    pstSoftWareParam->af32Bias[1][2] = 23;
    pstSoftWareParam->af32Bias[1][3] = 27;
    pstSoftWareParam->af32Bias[1][4] = 37;
    pstSoftWareParam->af32Bias[1][5] = 58;

    /*Malloc assist buffer memory*/
    u32ClassNum = pstSoftWareParam->u32ClassNum+1;

    SAMPLE_SVP_CHECK_EXPR_RET(SAMPLE_SVP_NNIE_YOLOV3_TINY_REPORT_BLOB_NUM != pstNnieParam->pstModel->astSeg[0].u16DstNum,
                              HI_FAILURE,SAMPLE_SVP_ERR_LEVEL_ERROR,"Error,pstNnieParam->pstModel->astSeg[0].u16DstNum(%d) should be %d!\n",
                              pstNnieParam->pstModel->astSeg[0].u16DstNum,SAMPLE_SVP_NNIE_YOLOV3_TINY_REPORT_BLOB_NUM);
    u32TmpBufTotalSize = SAMPLE_SVP_NNIE_Yolov3_tiny_GetResultTmpBuf(pstNnieParam,pstSoftWareParam);
    u32DstRoiSize = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*pstSoftWareParam->u32MaxRoiNum*sizeof(HI_U32)*SAMPLE_SVP_NNIE_COORDI_NUM);
    u32DstScoreSize = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*pstSoftWareParam->u32MaxRoiNum*sizeof(HI_U32));
    u32ClassRoiNumSize = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*sizeof(HI_U32));
    u32TotalSize = u32TotalSize+u32DstRoiSize+u32DstScoreSize+u32ClassRoiNumSize+u32TmpBufTotalSize;
    s32Ret = SAMPLE_COMM_SVP_MallocCached("SAMPLE_YOLOV3_INIT",NULL,(HI_U64*)&u64PhyAddr,
                                          (void**)&pu8VirAddr,u32TotalSize);
    SAMPLE_SVP_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
                              "Error,Malloc memory failed!\n");
    memset(pu8VirAddr,0, u32TotalSize);
    SAMPLE_COMM_SVP_FlushCache(u64PhyAddr,(void*)pu8VirAddr,u32TotalSize);

    /*set each tmp buffer addr*/
    pstSoftWareParam->stGetResultTmpBuf.u64PhyAddr = u64PhyAddr;
    pstSoftWareParam->stGetResultTmpBuf.u64VirAddr = (HI_U64)(pu8VirAddr);

    /*set result blob*/
    pstSoftWareParam->stDstRoi.enType = SVP_BLOB_TYPE_S32;
    pstSoftWareParam->stDstRoi.u64PhyAddr = u64PhyAddr+u32TmpBufTotalSize;
    pstSoftWareParam->stDstRoi.u64VirAddr = (HI_U64)(pu8VirAddr+u32TmpBufTotalSize);
    pstSoftWareParam->stDstRoi.u32Stride = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*
                                                                   pstSoftWareParam->u32MaxRoiNum*sizeof(HI_U32)*SAMPLE_SVP_NNIE_COORDI_NUM);
    pstSoftWareParam->stDstRoi.u32Num = 1;
    pstSoftWareParam->stDstRoi.unShape.stWhc.u32Chn = 1;
    pstSoftWareParam->stDstRoi.unShape.stWhc.u32Height = 1;
    pstSoftWareParam->stDstRoi.unShape.stWhc.u32Width = u32ClassNum*
            pstSoftWareParam->u32MaxRoiNum*SAMPLE_SVP_NNIE_COORDI_NUM;

    pstSoftWareParam->stDstScore.enType = SVP_BLOB_TYPE_S32;
    pstSoftWareParam->stDstScore.u64PhyAddr = u64PhyAddr+u32TmpBufTotalSize+u32DstRoiSize;
    pstSoftWareParam->stDstScore.u64VirAddr = (HI_U64)(pu8VirAddr+u32TmpBufTotalSize+u32DstRoiSize);
    pstSoftWareParam->stDstScore.u32Stride = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*
                                                                     pstSoftWareParam->u32MaxRoiNum*sizeof(HI_U32));
    pstSoftWareParam->stDstScore.u32Num = 1;
    pstSoftWareParam->stDstScore.unShape.stWhc.u32Chn = 1;
    pstSoftWareParam->stDstScore.unShape.stWhc.u32Height = 1;
    pstSoftWareParam->stDstScore.unShape.stWhc.u32Width = u32ClassNum*pstSoftWareParam->u32MaxRoiNum;

    pstSoftWareParam->stClassRoiNum.enType = SVP_BLOB_TYPE_S32;
    pstSoftWareParam->stClassRoiNum.u64PhyAddr = u64PhyAddr+u32TmpBufTotalSize+
            u32DstRoiSize+u32DstScoreSize;
    pstSoftWareParam->stClassRoiNum.u64VirAddr = (HI_U64)(pu8VirAddr+u32TmpBufTotalSize+
                                                          u32DstRoiSize+u32DstScoreSize);
    pstSoftWareParam->stClassRoiNum.u32Stride = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*sizeof(HI_U32));
    pstSoftWareParam->stClassRoiNum.u32Num = 1;
    pstSoftWareParam->stClassRoiNum.unShape.stWhc.u32Chn = 1;
    pstSoftWareParam->stClassRoiNum.unShape.stWhc.u32Height = 1;
    pstSoftWareParam->stClassRoiNum.unShape.stWhc.u32Width = u32ClassNum;

    return s32Ret;
}



HI_S32 SAMPLE_SVP_NNIE_Yolov3_tiny_Deinit(
        SAMPLE_SVP_NNIE_PARAM_S *pstNnieParam,
        SAMPLE_SVP_NNIE_YOLOV3_TINY_SOFTWARE_PARAM_S *pstSoftWareParam,
        SAMPLE_SVP_NNIE_MODEL_S *pstNnieModel)
{
    HI_S32 s32Ret = HI_SUCCESS;
    /*hardware deinit*/
    if(pstNnieParam!=NULL)
    {
        s32Ret = SAMPLE_COMM_SVP_NNIE_ParamDeinit(pstNnieParam);
        SAMPLE_SVP_CHECK_EXPR_TRACE(HI_SUCCESS != s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
                                    "Error,SAMPLE_COMM_SVP_NNIE_ParamDeinit failed!\n");
    }
    /*software deinit*/
    if(pstSoftWareParam!=NULL)
    {
        s32Ret = SAMPLE_SVP_NNIE_Yolov3_tiny_SoftwareDeinit(pstSoftWareParam);
        SAMPLE_SVP_CHECK_EXPR_TRACE(HI_SUCCESS != s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
                                    "Error,SAMPLE_SVP_NNIE_Yolov3_SoftwareDeinit failed!\n");
    }
    /*model deinit*/
    if(pstNnieModel!=NULL)
    {
        s32Ret = SAMPLE_COMM_SVP_NNIE_UnloadModel(pstNnieModel);
        SAMPLE_SVP_CHECK_EXPR_TRACE(HI_SUCCESS != s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
                                    "Error,SAMPLE_COMM_SVP_NNIE_UnloadModel failed!\n");
    }
    return s32Ret;
}

HI_S32 SAMPLE_SVP_NNIE_Yolov3_tiny_ParamInit(
        SAMPLE_SVP_NNIE_CFG_S *pstCfg,
        SAMPLE_SVP_NNIE_PARAM_S *pstNnieParam,
        SAMPLE_SVP_NNIE_YOLOV3_TINY_SOFTWARE_PARAM_S *pstSoftWareParam)
{
    HI_S32 s32Ret = HI_SUCCESS;
    /*init hardware para*/
    s32Ret = SAMPLE_COMM_SVP_NNIE_ParamInit(pstCfg,pstNnieParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(
                HI_SUCCESS != s32Ret,
                INIT_FAIL_0,
                SAMPLE_SVP_ERR_LEVEL_ERROR,
                "Error(%#x),SAMPLE_COMM_SVP_NNIE_ParamInit failed!\n",
                s32Ret);

    /*init software para*/
    s32Ret = SAMPLE_SVP_NNIE_Yolov3_tiny_SoftwareInit(
                pstCfg,
                pstNnieParam,
                pstSoftWareParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(
                HI_SUCCESS != s32Ret,
                INIT_FAIL_0,
                SAMPLE_SVP_ERR_LEVEL_ERROR,
                "Error(%#x),SAMPLE_SVP_NNIE_Yolov3_SoftwareInit failed!\n",
                s32Ret);

    return s32Ret;

INIT_FAIL_0:
    s32Ret = SAMPLE_SVP_NNIE_Yolov3_tiny_Deinit(pstNnieParam, pstSoftWareParam, NULL);
    SAMPLE_SVP_CHECK_EXPR_RET(
                HI_SUCCESS != s32Ret,
                s32Ret,
                SAMPLE_SVP_ERR_LEVEL_ERROR,
                "Error(%#x),SAMPLE_SVP_NNIE_Yolov3_Deinit failed!\n",
                s32Ret);
    return HI_FAILURE;
}

void deinit_yolov3tiny(
        SAMPLE_SVP_NNIE_MODEL_S *s_stYolov3Model_ipc,
        SAMPLE_SVP_NNIE_PARAM_S *s_stYolov3NnieParam_ipc,
        SAMPLE_SVP_NNIE_YOLOV3_TINY_SOFTWARE_PARAM_S *s_stYolov3SoftwareParam_ipc)
{
    SAMPLE_SVP_NNIE_Yolov3_Deinit(
                s_stYolov3NnieParam_ipc,
                s_stYolov3SoftwareParam_ipc,
                s_stYolov3Model_ipc);
    SAMPLE_COMM_SVP_CheckSysExit();
}

void NNIE_LOAD_MODEL_yolov3_tiny(
        SAMPLE_SVP_NNIE_CFG_S *stNnieCfg_ipc,
        SAMPLE_SVP_NNIE_MODEL_S *s_stYolov3Model_ipc,
        SAMPLE_SVP_NNIE_PARAM_S *s_stYolov3NnieParam_ipc,
        SAMPLE_SVP_NNIE_YOLOV3_TINY_SOFTWARE_PARAM_S *s_stYolov3SoftwareParam_ipc)
{
    HI_CHAR *pcModelName = "inst_yolov3_tiny_func.wk";
    HI_S32 s32Ret = HI_SUCCESS;

    /*Sys init*/
    //SAMPLE_COMM_SVP_CheckSysInit();
    /*Yolov3 Load model*/
    SAMPLE_SVP_TRACE_INFO("Yolov3 Load model!\n");
    s32Ret = SAMPLE_COMM_SVP_NNIE_LoadModel(pcModelName, s_stYolov3Model_ipc);
    SAMPLE_SVP_CHECK_EXPR_GOTO(
                HI_SUCCESS != s32Ret,
                YOLOV3_FAIL_0,
                SAMPLE_SVP_ERR_LEVEL_ERROR,
                "Error,SAMPLE_COMM_SVP_NNIE_LoadModel failed!\n");

    /*Yolov3 parameter initialization*/
    /*Yolov3 software parameters are set in SAMPLE_SVP_NNIE_Yolov3_SoftwareInit,
          if user has changed net struct, please make sure the parameter settings in
          SAMPLE_SVP_NNIE_Yolov3_SoftwareInit function are correct*/
    SAMPLE_SVP_TRACE_INFO("Yolov3 parameter initialization!\n");
    s_stYolov3NnieParam_ipc->pstModel = &s_stYolov3Model_ipc->stModel;
    s32Ret = SAMPLE_SVP_NNIE_Yolov3_tiny_ParamInit(stNnieCfg_ipc,s_stYolov3NnieParam_ipc,s_stYolov3SoftwareParam_ipc);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,YOLOV3_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
                               "Error,SAMPLE_SVP_NNIE_Yolov3_ParamInit failed!\n");
    return;

YOLOV3_FAIL_0:
    printf("NNIE_LOAD_MODEL_yolov3 failed\n");
    return;
}

void NNIE_process_yolov3_tiny(
        void *data,SAMPLE_SVP_NNIE_CFG_S *stNnieCfg_ipc,
        SAMPLE_SVP_NNIE_PARAM_S *s_stYolov3NnieParam_ipc,
        SAMPLE_SVP_NNIE_YOLOV3_TINY_SOFTWARE_PARAM_S *s_stYolov3SoftwareParam_ipc,
        DETECT_RET_BUF *buf_ret,
        int test_mode)
{
    SAMPLE_SVP_NNIE_INPUT_DATA_INDEX_S stInputDataIdx = {0};
    SAMPLE_SVP_NNIE_PROCESS_SEG_INDEX_S stProcSegIdx = {0};
    HI_FLOAT f32PrintResultThresh = 0.0f;
    /*Set configuration parameter*/
    f32PrintResultThresh = 0.3f;
    HI_S32 s32Ret = HI_SUCCESS;

    /*Fill src data*/
    if(!test_mode)
        SAMPLE_SVP_TRACE_INFO("Yolov3-tiny start!\n");
    stInputDataIdx.u32SegIdx = 0;
    stInputDataIdx.u32NodeIdx = 0;
    s32Ret = SAMPLE_SVP_NNIE_FillSrcData_ptr(data,s_stYolov3NnieParam_ipc,&stInputDataIdx);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,YOLOV3_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
                               "Error,SAMPLE_SVP_NNIE_FillSrcData failed!\n");

    /*NNIE process(process the 0-th segment)*/
    stProcSegIdx.u32SegIdx = 0;
    s32Ret = SAMPLE_SVP_NNIE_Forward(s_stYolov3NnieParam_ipc,&stInputDataIdx,&stProcSegIdx,HI_TRUE);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,YOLOV3_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
                               "Error,SAMPLE_SVP_NNIE_Forward failed!\n");

    if(!test_mode)
        return;
    /*Software process*/
    /*if user has changed net struct, please make sure SAMPLE_SVP_NNIE_Yolov3_GetResult
         function input datas are correct*/
    s32Ret = SAMPLE_SVP_NNIE_Yolov3_tiny_GetResult(s_stYolov3NnieParam_ipc,s_stYolov3SoftwareParam_ipc);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,YOLOV3_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
                               "Error,NNIE_process_yolov3_tiny failed!\n");

    /*print result, this sample has 81 classes:
          class 0:background	  class 1:person	   class 2:bicycle		   class 3:car			  class 4:motorbike 	 class 5:aeroplane
          class 6:bus			  class 7:train 	   class 8:truck		   class 9:boat 		  class 10:traffic light
          class 11:fire hydrant   class 12:stop sign   class 13:parking meter  class 14:bench		  class 15:bird
          class 16:cat			  class 17:dog		   class 18:horse		   class 19:sheep		  class 20:cow
          class 21:elephant 	  class 22:bear 	   class 23:zebra		   class 24:giraffe 	  class 25:backpack
          class 26:umbrella 	  class 27:handbag	   class 28:tie 		   class 29:suitcase	  class 30:frisbee
          class 31:skis 		  class 32:snowboard   class 33:sports ball    class 34:kite		  class 35:baseball bat
          class 36:baseball glove class 37:skateboard  class 38:surfboard	   class 39:tennis racket class 40bottle
          class 41:wine glass	  class 42:cup		   class 43:fork		   class 44:knife		  class 45:spoon
          class 46:bowl 		  class 47:banana	   class 48:apple		   class 49:sandwich	  class 50orange
          class 51:broccoli 	  class 52:carrot	   class 53:hot dog 	   class 54:pizza		  class 55:donut
          class 56:cake 		  class 57:chair	   class 58:sofa		   class 59:pottedplant   class 60bed
          class 61:diningtable	  class 62:toilet	   class 63:vmonitor	   class 64:laptop		  class 65:mouse
          class 66:remote		  class 67:keyboard    class 68:cell phone	   class 69:microwave	  class 70:oven
          class 71:toaster		  class 72:sink 	   class 73:refrigerator   class 74:book		  class 75:clock
          class 76:vase 		  class 77:scissors    class 78:teddy bear	   class 79:hair drier	  class 80:toothbrush*/
    //	SAMPLE_SVP_TRACE_INFO("Yolov3 result:\n");
    (void)SAMPLE_SVP_NNIE_Detection_Result(&s_stYolov3SoftwareParam_ipc->stDstScore,
                                           &s_stYolov3SoftwareParam_ipc->stDstRoi, &s_stYolov3SoftwareParam_ipc->stClassRoiNum,f32PrintResultThresh,buf_ret);

    return;

YOLOV3_FAIL_0:
    printf("NNIE_process_yolov3 failed\n");
    return;
}


void deinit_yolov3(
        SAMPLE_SVP_NNIE_MODEL_S *s_stYolov3Model_ipc,
        SAMPLE_SVP_NNIE_PARAM_S *s_stYolov3NnieParam_ipc,
        SAMPLE_SVP_NNIE_YOLOV3_SOFTWARE_PARAM_S *s_stYolov3SoftwareParam_ipc)
{
    SAMPLE_SVP_NNIE_Yolov3_Deinit(
                s_stYolov3NnieParam_ipc,
                s_stYolov3SoftwareParam_ipc,
                s_stYolov3Model_ipc);
    SAMPLE_COMM_SVP_CheckSysExit();
}

void NNIE_LOAD_MODEL_yolov3(
        SAMPLE_SVP_NNIE_CFG_S *stNnieCfg_ipc,
        SAMPLE_SVP_NNIE_MODEL_S *s_stYolov3Model_ipc,
        SAMPLE_SVP_NNIE_PARAM_S *s_stYolov3NnieParam_ipc,
        SAMPLE_SVP_NNIE_YOLOV3_SOFTWARE_PARAM_S *s_stYolov3SoftwareParam_ipc)
{
    HI_CHAR *pcModelName = "inst_yolov3_cycle.wk";
    HI_S32 s32Ret = HI_SUCCESS;
    /*Sys init*/
    //SAMPLE_COMM_SVP_CheckSysInit();

    /*Yolov3 Load model*/
    SAMPLE_SVP_TRACE_INFO("Yolov3 Load model!\n");
    s32Ret = SAMPLE_COMM_SVP_NNIE_LoadModel(pcModelName,s_stYolov3Model_ipc);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,YOLOV3_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
                               "Error,SAMPLE_COMM_SVP_NNIE_LoadModel failed!\n");

    /*Yolov3 parameter initialization*/
    /*Yolov3 software parameters are set in SAMPLE_SVP_NNIE_Yolov3_SoftwareInit,
          if user has changed net struct, please make sure the parameter settings in
          SAMPLE_SVP_NNIE_Yolov3_SoftwareInit function are correct*/
    SAMPLE_SVP_TRACE_INFO("Yolov3 parameter initialization!\n");
    s_stYolov3NnieParam_ipc->pstModel = &s_stYolov3Model_ipc->stModel;
    s32Ret = SAMPLE_SVP_NNIE_Yolov3_ParamInit(stNnieCfg_ipc,s_stYolov3NnieParam_ipc,s_stYolov3SoftwareParam_ipc);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,YOLOV3_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
                               "Error,SAMPLE_SVP_NNIE_Yolov3_ParamInit failed!\n");
    return;
YOLOV3_FAIL_0:
    printf("NNIE_LOAD_MODEL_yolov3 failed\n");
    return;

}

void NNIE_process_yolov3(
        void *data,
        SAMPLE_SVP_NNIE_CFG_S *stNnieCfg_ipc,
        SAMPLE_SVP_NNIE_PARAM_S *s_stYolov3NnieParam_ipc,
        SAMPLE_SVP_NNIE_YOLOV3_SOFTWARE_PARAM_S *s_stYolov3SoftwareParam_ipc,
        DETECT_RET_BUF *buf_ret,
        int test_mode)
{
    SAMPLE_SVP_NNIE_INPUT_DATA_INDEX_S stInputDataIdx = {0};
    SAMPLE_SVP_NNIE_PROCESS_SEG_INDEX_S stProcSegIdx = {0};
    HI_FLOAT f32PrintResultThresh = 0.0f;
    /*Set configuration parameter*/
    f32PrintResultThresh = 0.8f;
    HI_S32 s32Ret = HI_SUCCESS;

    /*Fill src data*/
    if(test_mode)
        SAMPLE_SVP_TRACE_INFO("Yolov3 start!\n");

    stInputDataIdx.u32SegIdx = 0;
    stInputDataIdx.u32NodeIdx = 0;
    s32Ret = SAMPLE_SVP_NNIE_FillSrcData_ptr(data,s_stYolov3NnieParam_ipc,&stInputDataIdx);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,YOLOV3_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
                               "Error,SAMPLE_SVP_NNIE_FillSrcData failed!\n");

    /*NNIE process(process the 0-th segment)*/
    stProcSegIdx.u32SegIdx = 0;
    s32Ret = SAMPLE_SVP_NNIE_Forward(s_stYolov3NnieParam_ipc,&stInputDataIdx,&stProcSegIdx,HI_TRUE);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,YOLOV3_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
                               "Error,SAMPLE_SVP_NNIE_Forward failed!\n");

    if(!test_mode)
        return;

    /*Software process*/
    /*if user has changed net struct, please make sure SAMPLE_SVP_NNIE_Yolov3_GetResult
         function input datas are correct*/
    s32Ret = SAMPLE_SVP_NNIE_Yolov3_GetResult(s_stYolov3NnieParam_ipc,s_stYolov3SoftwareParam_ipc);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,YOLOV3_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
                               "Error,SAMPLE_SVP_NNIE_Yolov3_GetResult failed!\n");
    /*print result, this sample has 81 classes:
          class 0:background	  class 1:person	   class 2:bicycle		   class 3:car			  class 4:motorbike 	 class 5:aeroplane
          class 6:bus			  class 7:train 	   class 8:truck		   class 9:boat 		  class 10:traffic light
          class 11:fire hydrant   class 12:stop sign   class 13:parking meter  class 14:bench		  class 15:bird
          class 16:cat			  class 17:dog		   class 18:horse		   class 19:sheep		  class 20:cow
          class 21:elephant 	  class 22:bear 	   class 23:zebra		   class 24:giraffe 	  class 25:backpack
          class 26:umbrella 	  class 27:handbag	   class 28:tie 		   class 29:suitcase	  class 30:frisbee
          class 31:skis 		  class 32:snowboard   class 33:sports ball    class 34:kite		  class 35:baseball bat
          class 36:baseball glove class 37:skateboard  class 38:surfboard	   class 39:tennis racket class 40bottle
          class 41:wine glass	  class 42:cup		   class 43:fork		   class 44:knife		  class 45:spoon
          class 46:bowl 		  class 47:banana	   class 48:apple		   class 49:sandwich	  class 50orange
          class 51:broccoli 	  class 52:carrot	   class 53:hot dog 	   class 54:pizza		  class 55:donut
          class 56:cake 		  class 57:chair	   class 58:sofa		   class 59:pottedplant   class 60bed
          class 61:diningtable	  class 62:toilet	   class 63:vmonitor	   class 64:laptop		  class 65:mouse
          class 66:remote		  class 67:keyboard    class 68:cell phone	   class 69:microwave	  class 70:oven
          class 71:toaster		  class 72:sink 	   class 73:refrigerator   class 74:book		  class 75:clock
          class 76:vase 		  class 77:scissors    class 78:teddy bear	   class 79:hair drier	  class 80:toothbrush*/
    (void)SAMPLE_SVP_NNIE_Detection_Result(&s_stYolov3SoftwareParam_ipc->stDstScore,
                                           &s_stYolov3SoftwareParam_ipc->stDstRoi, &s_stYolov3SoftwareParam_ipc->stClassRoiNum,f32PrintResultThresh,buf_ret);
    return;
YOLOV3_FAIL_0:
    printf("NNIE_process_yolov3 failed\n");
    return;


}


/******************************************************************************
* function : show YOLOV3 sample(image 416x416 U8_C3)
******************************************************************************/
void SAMPLE_SVP_NNIE_Yolov3_tiny(void)
{
    HI_CHAR *pcSrcFile = "./data/nnie_image/rgb_planar/dog_bike_car_416x416.bgr";
    HI_CHAR *pcModelName = "./data/nnie_model/detection/inst_yolov3_tiny_func.wk";
    HI_U32 u32PicNum = 1;
    HI_FLOAT f32PrintResultThresh = 0.0f;
    HI_S32 s32Ret = HI_SUCCESS;
    SAMPLE_SVP_NNIE_CFG_S   stNnieCfg = {0};
    SAMPLE_SVP_NNIE_INPUT_DATA_INDEX_S stInputDataIdx = {0};
    SAMPLE_SVP_NNIE_PROCESS_SEG_INDEX_S stProcSegIdx = {0};

    /*Set configuration parameter*/
    f32PrintResultThresh = 0.2f;
    stNnieCfg.pszPic= pcSrcFile;
    stNnieCfg.u32MaxInputNum = u32PicNum; //max input image num in each batch
    stNnieCfg.u32MaxRoiNum = 0;
    stNnieCfg.aenNnieCoreId[0] = SVP_NNIE_ID_0;//set NNIE core

    /*Sys init*/
    //  SAMPLE_COMM_SVP_CheckSysInit();

    /*Yolov3 Load model*/
    SAMPLE_SVP_TRACE_INFO("Yolov3 Load model!\n");
    s32Ret = SAMPLE_COMM_SVP_NNIE_LoadModel(pcModelName,&s_stYolov3tinyModel);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,YOLOV3_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
                               "Error,SAMPLE_COMM_SVP_NNIE_LoadModel failed!\n");

    /*Yolov3 parameter initialization*/
    /*Yolov3 software parameters are set in SAMPLE_SVP_NNIE_Yolov3_SoftwareInit,
      if user has changed net struct, please make sure the parameter settings in
      SAMPLE_SVP_NNIE_Yolov3_SoftwareInit function are correct*/
    SAMPLE_SVP_TRACE_INFO("Yolov3 parameter initialization!\n");
    s_stYolov3tinyNnieParam.pstModel = &s_stYolov3tinyModel.stModel;
    s32Ret = SAMPLE_SVP_NNIE_Yolov3_tiny_ParamInit(&stNnieCfg,&s_stYolov3tinyNnieParam,&s_stYolov3tinySoftwareParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,YOLOV3_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
                               "Error,SAMPLE_SVP_NNIE_Yolov3_ParamInit failed!\n");

    /*Fill src data*/
    SAMPLE_SVP_TRACE_INFO("Yolov3 start!\n");
    stInputDataIdx.u32SegIdx = 0;
    stInputDataIdx.u32NodeIdx = 0;
    s32Ret = SAMPLE_SVP_NNIE_FillSrcData(&stNnieCfg,&s_stYolov3tinyNnieParam,&stInputDataIdx);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,YOLOV3_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
                               "Error,SAMPLE_SVP_NNIE_FillSrcData failed!\n");

    /*NNIE process(process the 0-th segment)*/
    stProcSegIdx.u32SegIdx = 0;
    s32Ret = SAMPLE_SVP_NNIE_Forward(&s_stYolov3tinyNnieParam,&stInputDataIdx,&stProcSegIdx,HI_TRUE);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,YOLOV3_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
                               "Error,SAMPLE_SVP_NNIE_Forward failed!\n");

    /*Software process*/
    /*if user has changed net struct, please make sure SAMPLE_SVP_NNIE_Yolov3_GetResult
     function input datas are correct*/
    s32Ret = SAMPLE_SVP_NNIE_Yolov3_tiny_GetResult(&s_stYolov3tinyNnieParam,&s_stYolov3tinySoftwareParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,YOLOV3_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
                               "Error,SAMPLE_SVP_NNIE_Yolov3_GetResult failed!\n");

    /*print result, this sample has 81 classes:
      class 0:background      class 1:person       class 2:bicycle         class 3:car            class 4:motorbike      class 5:aeroplane
      class 6:bus             class 7:train        class 8:truck           class 9:boat           class 10:traffic light
      class 11:fire hydrant   class 12:stop sign   class 13:parking meter  class 14:bench         class 15:bird
      class 16:cat            class 17:dog         class 18:horse          class 19:sheep         class 20:cow
      class 21:elephant       class 22:bear        class 23:zebra          class 24:giraffe       class 25:backpack
      class 26:umbrella       class 27:handbag     class 28:tie            class 29:suitcase      class 30:frisbee
      class 31:skis           class 32:snowboard   class 33:sports ball    class 34:kite          class 35:baseball bat
      class 36:baseball glove class 37:skateboard  class 38:surfboard      class 39:tennis racket class 40bottle
      class 41:wine glass     class 42:cup         class 43:fork           class 44:knife         class 45:spoon
      class 46:bowl           class 47:banana      class 48:apple          class 49:sandwich      class 50orange
      class 51:broccoli       class 52:carrot      class 53:hot dog        class 54:pizza         class 55:donut
      class 56:cake           class 57:chair       class 58:sofa           class 59:pottedplant   class 60bed
      class 61:diningtable    class 62:toilet      class 63:vmonitor       class 64:laptop        class 65:mouse
      class 66:remote         class 67:keyboard    class 68:cell phone     class 69:microwave     class 70:oven
      class 71:toaster        class 72:sink        class 73:refrigerator   class 74:book          class 75:clock
      class 76:vase           class 77:scissors    class 78:teddy bear     class 79:hair drier    class 80:toothbrush*/
    SAMPLE_SVP_TRACE_INFO("Yolov3 result:\n");
    (void)SAMPLE_SVP_NNIE_Detection_PrintResult(&s_stYolov3tinySoftwareParam.stDstScore,
                                                &s_stYolov3tinySoftwareParam.stDstRoi, &s_stYolov3tinySoftwareParam.stClassRoiNum,f32PrintResultThresh);


YOLOV3_FAIL_0:
    SAMPLE_SVP_NNIE_Yolov3_tiny_Deinit(&s_stYolov3tinyNnieParam,&s_stYolov3tinySoftwareParam,&s_stYolov3tinyModel);
    SAMPLE_COMM_SVP_CheckSysExit();
}

void SAMPLE_SVP_NNIE_Yolov3_tiny_HandleSig(void)
{
    SAMPLE_SVP_NNIE_Yolov3_tiny_Deinit(&s_stYolov3tinyNnieParam,&s_stYolov3tinySoftwareParam,&s_stYolov3tinyModel);
    memset(&s_stYolov3tinyNnieParam,0,sizeof(SAMPLE_SVP_NNIE_PARAM_S));
    memset(&s_stYolov3tinySoftwareParam,0,sizeof(SAMPLE_SVP_NNIE_YOLOV3_TINY_SOFTWARE_PARAM_S));
    memset(&s_stYolov3tinyModel,0,sizeof(SAMPLE_SVP_NNIE_MODEL_S));
    SAMPLE_COMM_SVP_CheckSysExit();
}


void NNIE_Yolov3_tiny_HandleSig(SAMPLE_SVP_NNIE_MODEL_S *s_stYolov3Model_ipc,
                                SAMPLE_SVP_NNIE_PARAM_S *s_stYolov3NnieParam_ipc,SAMPLE_SVP_NNIE_YOLOV3_TINY_SOFTWARE_PARAM_S *s_stYolov3SoftwareParam_ipc)
{
    SAMPLE_SVP_NNIE_Yolov3_tiny_Deinit(s_stYolov3NnieParam_ipc,s_stYolov3SoftwareParam_ipc,s_stYolov3Model_ipc);
    memset(s_stYolov3NnieParam_ipc,0,sizeof(SAMPLE_SVP_NNIE_PARAM_S));
    memset(s_stYolov3SoftwareParam_ipc,0,sizeof(SAMPLE_SVP_NNIE_YOLOV3_SOFTWARE_PARAM_S));
    memset(s_stYolov3Model_ipc,0,sizeof(SAMPLE_SVP_NNIE_MODEL_S));
    SAMPLE_COMM_SVP_CheckSysExit();
}



void NNIE_Yolov3_HandleSig(SAMPLE_SVP_NNIE_MODEL_S *s_stYolov3Model_ipc,
                           SAMPLE_SVP_NNIE_PARAM_S *s_stYolov3NnieParam_ipc,SAMPLE_SVP_NNIE_YOLOV3_SOFTWARE_PARAM_S *s_stYolov3SoftwareParam_ipc)
{
    SAMPLE_SVP_NNIE_Yolov3_Deinit(s_stYolov3NnieParam_ipc,s_stYolov3SoftwareParam_ipc,s_stYolov3Model_ipc);
    memset(s_stYolov3NnieParam_ipc,0,sizeof(SAMPLE_SVP_NNIE_PARAM_S));
    memset(s_stYolov3SoftwareParam_ipc,0,sizeof(SAMPLE_SVP_NNIE_YOLOV3_SOFTWARE_PARAM_S));
    memset(s_stYolov3Model_ipc,0,sizeof(SAMPLE_SVP_NNIE_MODEL_S));
    SAMPLE_COMM_SVP_CheckSysExit();
}


#if 1
//loadmod - inference - GetReslut
//stNnieCfg must fill these members:
//		stNnieCfg_ipc->pszPic= 0;
//		stNnieCfg_ipc->u32MaxInputNum = u32PicNum; //max input image num in each batch
//		stNnieCfg_ipc->u32MaxRoiNum = 0;
//		stNnieCfg_ipc->aenNnieCoreId[0] = hw_id;//set NNIE core

void NNIE_LOAD_MODEL(
        SAMPLE_SVP_NNIE_CFG_S *stNnieCfg,
        SAMPLE_SVP_NNIE_MODEL_S *s_Model,
        SAMPLE_SVP_NNIE_PARAM_S *s_NnieParam,
        const char *pcModelName)
{
    HI_S32 s32Ret = HI_SUCCESS;
    /*Sys init*/
    //SAMPLE_COMM_SVP_CheckSysInit();
    /*Load model*/
    SAMPLE_SVP_TRACE_INFO("Load model!\n");
    s32Ret = SAMPLE_COMM_SVP_NNIE_LoadModel(pcModelName,s_Model);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,LOAD_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
                               "Error,SVP_NNIE_LoadModel failed!\n");

    SAMPLE_SVP_TRACE_INFO("parameter initialization!\n");
    s_NnieParam->pstModel = &s_Model->stModel;


    s32Ret = SAMPLE_COMM_SVP_NNIE_ParamInit(stNnieCfg,s_NnieParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,INIT_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
                               "Error(%#x),SVP_NNIE_ParamInit failed!\n",s32Ret);


    /*
    s32Ret = SAMPLE_SVP_NNIE_Yolov3_tiny_ParamInit(stNnieCfg_ipc,s_stYolov3NnieParam_ipc,s_stYolov3SoftwareParam_ipc);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,YOLOV3_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_Yolov3_ParamInit failed!\n");*/
    return;
INIT_FAIL_0:
    if(s_NnieParam!=NULL)
    {
        s32Ret = SAMPLE_COMM_SVP_NNIE_ParamDeinit(s_NnieParam);
        SAMPLE_SVP_CHECK_EXPR_TRACE(HI_SUCCESS != s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
                                    "Error,SVP_NNIE_ParamDeinit failed!\n");
    }
    /*model deinit*/
    if(s_Model!=NULL)
    {
        s32Ret = SAMPLE_COMM_SVP_NNIE_UnloadModel(s_Model);
        SAMPLE_SVP_CHECK_EXPR_TRACE(HI_SUCCESS != s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
                                    "Error,SVP_NNIE_UnloadModel failed!\n");
    }
LOAD_FAIL_0:
    printf("NNIE_LOAD_MODEL failed\n");
    return;

}

void NNIE_INFERENCE(void *data,SAMPLE_SVP_NNIE_CFG_S *stNnieCfg,SAMPLE_SVP_NNIE_PARAM_S *s_NnieParam)
{
    SAMPLE_SVP_NNIE_INPUT_DATA_INDEX_S stInputDataIdx = {0};
    SAMPLE_SVP_NNIE_PROCESS_SEG_INDEX_S stProcSegIdx = {0};
    /*Set configuration parameter*/
    HI_S32 s32Ret = HI_SUCCESS;
    /*Fill src data*/
    //	SAMPLE_SVP_TRACE_INFO("Yolov3 start!\n");
    stInputDataIdx.u32SegIdx = 0;
    stInputDataIdx.u32NodeIdx = 0;
    s32Ret = SAMPLE_SVP_NNIE_FillSrcData_ptr(data,s_NnieParam,&stInputDataIdx);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,IF_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
                               "Error,SAMPLE_SVP_NNIE_FillSrcData failed!\n");

    /*NNIE process(process the 0-th segment)*/
    stProcSegIdx.u32SegIdx = 0;
    s32Ret = SAMPLE_SVP_NNIE_Forward(s_NnieParam,&stInputDataIdx,&stProcSegIdx,HI_TRUE);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,IF_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
                               "Error,SAMPLE_SVP_NNIE_Forward failed!\n");
    return;
IF_FAIL_0:
    printf("NNIE_INFERENCE failed\n");
    return;
}

void NNIE_GET_OUT_BUF(SAMPLE_SVP_NNIE_PARAM_S*pstNnieParam,nnie_out_buf *buf,int num)
{
    int i = 0;
    for(i = 0 ;i <num ; i++){
        buf->buf_addr[i]= (HI_S32*)pstNnieParam->astSegData[0].astDst[0].u64VirAddr;
        buf->stride[i]= pstNnieParam->astSegData[0].astDst[0].u32Stride;
    }
}

void NNIE_Relese(SAMPLE_SVP_NNIE_MODEL_S *s_Model, SAMPLE_SVP_NNIE_PARAM_S *s_NnieParam)
{
    HI_S32 s32Ret = HI_SUCCESS;
    if(s_NnieParam!=NULL)
    {
        s32Ret = SAMPLE_COMM_SVP_NNIE_ParamDeinit(s_NnieParam);
        SAMPLE_SVP_CHECK_EXPR_TRACE(HI_SUCCESS != s32Ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
                                    "Error,SVP_NNIE_ParamDeinit failed!\n");
    }
    /*model deinit*/
    if(s_Model!=NULL)
    {
        s32Ret = SAMPLE_COMM_SVP_NNIE_UnloadModel(s_Model);
        SAMPLE_SVP_CHECK_EXPR_TRACE(HI_SUCCESS != s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
                                    "Error,SVP_NNIE_UnloadModel failed!\n");
    }
    memset(s_NnieParam,0,sizeof(SAMPLE_SVP_NNIE_PARAM_S));
    memset(s_Model,0,sizeof(SAMPLE_SVP_NNIE_MODEL_S));
}
#endif


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
