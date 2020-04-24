#ifndef SAMPLE_NNIE_EXT_H
#define SAMPLE_NNIE_EXT_H

#include "hi_type.h"
#include "hi_common.h"
#include "hi_comm_sys.h"
#include "hi_comm_svp.h"
#include "sample_comm.h"
#include "sample_comm_svp.h"
#include "sample_comm_nnie.h"
#include "sample_nnie_main.h"
#include "sample_svp_nnie_software.h"
#include "sample_comm_ive.h"
#include "sample_nnie_main.h"
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
#include <sys/prctl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/time.h>

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define NNIE_MAX_OUT_BUF 10
typedef struct NNIE_OUT_BUF_ {
    void *buf_addr[NNIE_MAX_OUT_BUF];
    unsigned int stride[NNIE_MAX_OUT_BUF];
} nnie_out_buf;

typedef struct retbuf {
    int ret_count;
    char class_ret[MAX_RET_COUNT];
    int x0[MAX_RET_COUNT];
    int x1[MAX_RET_COUNT];
    int y0[MAX_RET_COUNT];
    int y1[MAX_RET_COUNT];
} DETECT_RET_BUF;

HI_S32 SAMPLE_SVP_NNIE_FillSrcData(SAMPLE_SVP_NNIE_CFG_S *pstNnieCfg, SAMPLE_SVP_NNIE_PARAM_S *pstNnieParam, SAMPLE_SVP_NNIE_INPUT_DATA_INDEX_S *pstInputDataIdx);
void NNIE_LOAD_MODEL(
        SAMPLE_SVP_NNIE_CFG_S *stNnieCfg,
        SAMPLE_SVP_NNIE_MODEL_S *s_Model,
        SAMPLE_SVP_NNIE_PARAM_S *s_NnieParam,
        const char *pcModelName);

void NNIE_INFERENCE(
        void *data,
        SAMPLE_SVP_NNIE_CFG_S *stNnieCfg,
        SAMPLE_SVP_NNIE_PARAM_S *s_NnieParam);

void NNIE_Relese(
        SAMPLE_SVP_NNIE_MODEL_S *s_Model,
        SAMPLE_SVP_NNIE_PARAM_S *s_NnieParam);


void NNIE_GET_OUT_BUF(
        SAMPLE_SVP_NNIE_PARAM_S *pstNnieParam,
        nnie_out_buf *buf,
        int num);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif // SAMPLE_NNIE_EXT_H
