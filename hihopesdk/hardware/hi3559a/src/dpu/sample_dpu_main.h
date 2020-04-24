#ifndef __SAMPLE_DPU_MAIN_H__
#define __SAMPLE_DPU_MAIN_H__

#include "hi_types.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

HI_S32 dpu_init(int left_id,int right_id, int need_sysinit);

HI_VOID dpu_stop(int need_sysinit);

/* This case only for function design reference */
int dpu_read(char* rgb_buf,char* depth_buf);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __SAMPLE_DPU_MAIN_H__ */
