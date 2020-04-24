#include <atomic>
#include <iostream>
#include <vector>
#include <mutex>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/prctl.h>
#include "sample_comm.h"
#include "mpi_snap.h"
#include "../utils/utils.h"

#define MAX_MIPI_DEV  2

static std::mutex sMutex;
static SAMPLE_VI_CONFIG_S sViConfig;
static const VI_DEV ViDev[MAX_MIPI_DEV] = {0, 1};
static const VI_PIPE MppPipe[MAX_MIPI_DEV] = {0, 1};

static void getDefaulteViConfig(SAMPLE_VI_CONFIG_S &stViConfig) {
    SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);
    for (int i = 0; i < MAX_MIPI_DEV; ++i) {
        stViConfig.as32WorkingViId[i] = i;
        stViConfig.astViInfo[i].stSnsInfo.MipiDev = SAMPLE_COMM_VI_GetComboDevBySensor(stViConfig.astViInfo[i].stSnsInfo.enSnsType, i);
        stViConfig.astViInfo[i].stSnsInfo.s32BusId = i;
        stViConfig.astViInfo[i].stDevInfo.ViDev = ViDev[i];
        stViConfig.astViInfo[i].stDevInfo.enWDRMode = WDR_MODE_NONE;
        stViConfig.astViInfo[i].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
        stViConfig.astViInfo[i].stPipeInfo.aPipe[0] = MppPipe[i];
        stViConfig.astViInfo[i].stPipeInfo.aPipe[1] = -1;
        stViConfig.astViInfo[i].stPipeInfo.aPipe[2] = -1;
        stViConfig.astViInfo[i].stPipeInfo.aPipe[3] = -1;
        stViConfig.astViInfo[i].stChnInfo.ViChn = 0;
        stViConfig.astViInfo[i].stChnInfo.enPixFormat = PIXEL_FORMAT_RGB_888;
        stViConfig.astViInfo[i].stChnInfo.enDynamicRange = DYNAMIC_RANGE_SDR8;
        stViConfig.astViInfo[i].stChnInfo.enVideoFormat = VIDEO_FORMAT_LINEAR;
        stViConfig.astViInfo[i].stChnInfo.enCompressMode = COMPRESS_MODE_NONE;
    }
}
#if 0
static void getDefaulteViVbConfig(SAMPLE_VI_CONFIG_S &stViConfig, VB_CONFIG_S &stVbConf) {
    HI_S32 s32Ret;
    SIZE_S stSize;
    PIC_SIZE_E enPicSize;
    s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[0].stSnsInfo.enSnsType, &enPicSize);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_GetSizeBySensor failed with %d!\n", s32Ret);
        return ;
    }

    s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed with %d!\n", s32Ret);
        return ;
    }

    stVbConf.u32MaxPoolCnt = 2;
    stVbConf.astCommPool[0].u32BlkCnt = 20;
    stVbConf.astCommPool[0].u64BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, SAMPLE_PIXEL_FORMAT, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
    stVbConf.astCommPool[1].u32BlkCnt = 20;
    stVbConf.astCommPool[1].u64BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, DEFAULT_ALIGN);;
}

static bool viConfigVb(SAMPLE_VI_CONFIG_S &stViConfig) {
    HI_S32 s32Ret;
    VB_CONFIG_S stVbConf;
    getDefaulteViVbConfig(stViConfig, stVbConf);
    if (stVbConf.u32MaxPoolCnt == 0) {
        return false;
    };

    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
    if (s32Ret != HI_SUCCESS) {
        SAMPLE_PRT("system init failed with %d!\n", s32Ret);
        SAMPLE_COMM_SYS_Exit();
    }
    return s32Ret;
}
#endif
bool viStart(int dev) {
    getDefaulteViConfig(sViConfig);

    HI_S32 s32ViCnt = 0;
    for (int i = 0; i < 4; ++i)
    {
        if (dev & (0x1 << i)) {
            sViConfig.as32WorkingViId[s32ViCnt] = i;
            sViConfig.astViInfo[i].stPipeInfo.aPipe[i] = MppPipe[s32ViCnt];
            s32ViCnt++;
        }
    }
    sViConfig.s32WorkingViNum = s32ViCnt;

  //  if (viConfigVb(sViConfig) == false) {
   //     return false;
  //  }

    HI_S32 s32Ret = SAMPLE_COMM_VI_StartVi(&sViConfig);
    if (s32Ret != HI_SUCCESS) {
        SAMPLE_PRT("SAMPLE_COMM_VI_StartVi failed with %d!\n", s32Ret);
        SAMPLE_COMM_VI_StopVi(&sViConfig);
        return false;
    }

    VI_DUMP_ATTR_S stDumpAttr;
    stDumpAttr.bEnable = HI_TRUE;
    stDumpAttr.u32Depth = 3;
    stDumpAttr.enDumpType = VI_DUMP_TYPE_RAW;
    s32Ret = HI_MPI_VI_SetPipeDumpAttr(0, &stDumpAttr);
    if (HI_SUCCESS != s32Ret) {
        SAMPLE_PRT("HI_MPI_VI_SetPipeDumpAttr failed with %#x\n", s32Ret);
        SAMPLE_COMM_VI_StopVi(&sViConfig);
    }
    return true;
}

bool viStop() {
    SAMPLE_COMM_VI_StopVi(&sViConfig);
}


