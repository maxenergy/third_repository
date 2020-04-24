
#include <stdio.h>
#include <dlfcn.h>
#include "mpi_nnie.h"
static void * libm_handle = NULL;
//float (*cosf_method)(float);
//const char* (*getVersion_method)(void);


HI_S32 (*HI_MPI_SVP_NNIE_LoadModel_m)(const SVP_SRC_MEM_INFO_S *, SVP_NNIE_MODEL_S *);
HI_S32 (*HI_MPI_SVP_NNIE_GetTskBufSize_m)(HI_U32 , HI_U32 ,
	const SVP_NNIE_MODEL_S *, HI_U32*, HI_U32);
//       const SVP_NNIE_MODEL_S *, HI_U32 au32TskBufSize[], HI_U32 u32NetSegNum);
HI_S32 (*HI_MPI_SVP_NNIE_Forward_m)(SVP_NNIE_HANDLE *,
    const SVP_SRC_BLOB_S*, const SVP_NNIE_MODEL_S *,const SVP_DST_BLOB_S*,
    const SVP_NNIE_FORWARD_CTRL_S *,HI_BOOL);

//    const SVP_SRC_BLOB_S astSrc[], const SVP_NNIE_MODEL_S *pstModel,const SVP_DST_BLOB_S astDst[],
//    HI_S32 HI_MPI_SVP_NNIE_ForwardWithBbox(SVP_NNIE_HANDLE *phSvpNnieHandle,
//    const SVP_SRC_BLOB_S astSrc[], const SVP_SRC_BLOB_S astBbox[], const SVP_NNIE_MODEL_S *pstModel,
//    const SVP_DST_BLOB_S astDst[], const SVP_NNIE_FORWARD_WITHBBOX_CTRL_S *pstForwardCtrl,HI_BOOL bInstant);

 HI_S32 (*HI_MPI_SVP_NNIE_ForwardWithBbox_m)(SVP_NNIE_HANDLE *,
	 const SVP_SRC_BLOB_S*, const SVP_SRC_BLOB_S*, const SVP_NNIE_MODEL_S *,
	 const SVP_DST_BLOB_S*, const SVP_NNIE_FORWARD_WITHBBOX_CTRL_S *,HI_BOOL );


HI_S32 (*HI_MPI_SVP_NNIE_UnloadModel_m)(SVP_NNIE_MODEL_S *);
HI_S32 (*HI_MPI_SVP_NNIE_Query_m)(SVP_NNIE_ID_E ,SVP_NNIE_HANDLE ,HI_BOOL *,HI_BOOL );
HI_S32 (*HI_MPI_SVP_NNIE_AddTskBuf_m)(const SVP_MEM_INFO_S* );
HI_S32 (*HI_MPI_SVP_NNIE_RemoveTskBuf_m)(const SVP_MEM_INFO_S* );


static int libfunc_init_s2=0;

int init_method_from_lib()
{
	char *errorInfo;
	float result;
  
	if(libfunc_init_s2==1)
		return 0;
	libm_handle = dlopen("/home/kirin/lib/face/lib/libnnie_hisi.so", RTLD_LAZY );
	// 如果返回 NULL 句柄，表示无法找到对象文件，过程结束。否则的话，将会得到对象的一个句柄，可以进一步询问对象
	if (!libm_handle){
	// 如果返回 NULL 句柄,通过dlerror方法可以取得无法访问对象的原因
	printf("Open Error:%s.\n",dlerror());
	return 0;
	}

	// 使用 dlsym 函数，尝试解析新打开的对象文件中的符号。您将会得到一个有效的指向该符号的指针，或者是得到一个 NULL 并返回一个错误
	HI_MPI_SVP_NNIE_LoadModel_m = dlsym(libm_handle,"HI_MPI_SVP_NNIE_LoadModel");
	errorInfo = dlerror();// 调用dlerror方法，返回错误信息的同时，内存中的错误信息被清空
	if (errorInfo != NULL){
	printf("Dlsym Error:%s.\n",errorInfo);
	return 0;
	}

	HI_MPI_SVP_NNIE_GetTskBufSize_m = dlsym(libm_handle,"HI_MPI_SVP_NNIE_GetTskBufSize");
	errorInfo = dlerror();// 调用dlerror方法，返回错误信息的同时，内存中的错误信息被清空
	if (errorInfo != NULL){
	printf("Dlsym Error:%s.\n",errorInfo);
	return 0;
	}


	HI_MPI_SVP_NNIE_Forward_m = dlsym(libm_handle,"HI_MPI_SVP_NNIE_Forward");
	errorInfo = dlerror();// 调用dlerror方法，返回错误信息的同时，内存中的错误信息被清空
	if (errorInfo != NULL){
	printf("Dlsym Error:%s.\n",errorInfo);
	return 0;
	}


	HI_MPI_SVP_NNIE_ForwardWithBbox_m = dlsym(libm_handle,"HI_MPI_SVP_NNIE_ForwardWithBbox");
	errorInfo = dlerror();// 调用dlerror方法，返回错误信息的同时，内存中的错误信息被清空
	if (errorInfo != NULL){
	printf("Dlsym Error:%s.\n",errorInfo);
	return 0;
	}


	HI_MPI_SVP_NNIE_UnloadModel_m = dlsym(libm_handle,"HI_MPI_SVP_NNIE_UnloadModel");
	errorInfo = dlerror();// 调用dlerror方法，返回错误信息的同时，内存中的错误信息被清空
	if (errorInfo != NULL){
	printf("Dlsym Error:%s.\n",errorInfo);
	return 0;
	}


	HI_MPI_SVP_NNIE_Query_m = dlsym(libm_handle,"HI_MPI_SVP_NNIE_Query");
	errorInfo = dlerror();// 调用dlerror方法，返回错误信息的同时，内存中的错误信息被清空
	if (errorInfo != NULL){
	printf("Dlsym Error:%s.\n",errorInfo);
	return 0;
	}



	HI_MPI_SVP_NNIE_AddTskBuf_m = dlsym(libm_handle,"HI_MPI_SVP_NNIE_AddTskBuf");
	errorInfo = dlerror();// 调用dlerror方法，返回错误信息的同时，内存中的错误信息被清空
	if (errorInfo != NULL){
	printf("Dlsym Error:%s.\n",errorInfo);
	return 0;
	}

	HI_MPI_SVP_NNIE_RemoveTskBuf_m = dlsym(libm_handle,"HI_MPI_SVP_NNIE_RemoveTskBuf");
	errorInfo = dlerror();// 调用dlerror方法，返回错误信息的同时，内存中的错误信息被清空
	if (errorInfo != NULL){
	printf("Dlsym Error:%s.\n",errorInfo);
	return 0;
	}
	libfunc_init_s2 = 1;
return 0;
}

HI_S32 HI_MPI_SVP_NNIE_LoadModel(const SVP_SRC_MEM_INFO_S *pstModelBuf, SVP_NNIE_MODEL_S *pstModel){
	HI_S32 result;
	init_method_from_lib();
	result = (*HI_MPI_SVP_NNIE_LoadModel_m)(pstModelBuf,pstModel);
	return result;
}

/*****************************************************************************
*   Prototype    : HI_MPI_SVP_NNIE_GetTskBufSize
*   Description  : Get task buffer size
*   Parameters   : HI_U32               u32MaxInputNum       Maximum input num,  CNN_Forword input data num cann't
*                                                            be more than it.
*                  HI_U32               u32MaxBboxNum        Maximum Bbox num, the RPN's output bbox num, should
*                                                            be less or   equal to the compiler's correspond value.
*                  SVP_NNIE_MODEL_S     *pstModel            the  model from Loadmodel.
*                  HI_U32               au32TskBufSize[]     The task relate   auxiliary buffer array.
*                  HI_U32               u32NetSegNum         the au32TskBufSize array element num.
*   Return Value: HI_SUCCESS: Success; Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2017-10-06
*          Author        :
*          Modification  : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_SVP_NNIE_GetTskBufSize(HI_U32 u32MaxInputNum, HI_U32 u32MaxBboxNum,
       const SVP_NNIE_MODEL_S *pstModel, HI_U32 au32TskBufSize[], HI_U32 u32NetSegNum){
	HI_S32 result;
	
	init_method_from_lib();
	result = (*HI_MPI_SVP_NNIE_GetTskBufSize_m)(u32MaxInputNum,u32MaxBboxNum,pstModel,au32TskBufSize,u32NetSegNum);
	return result;
}

/*****************************************************************************
*   Prototype    : HI_MPI_SVP_NNIE_Forward
*   Description  : Perform CNN prediction on input sample(s), and output responses for corresponding sample(s)
*   Parameters   : SVP_NNIE_HANDLE                  *phSvpNnieHandle      Returned handle ID of a task
*                  const SVP_SRC_BLOB_S                    astSrc[]             Input node array.
*                  const SVP_NNIE_MODEL_S                  pstModel             CNN model data
*                  const SVP_DST_BLOB_S                    astDst[]             Output node array
*                  const SVP_NNIE_FORWARD_CTRL_S          *pstForwardCtrl       Ctrl prameters
*                  HI_BOOL                           bInstant             Flag indicating whether to generate an interrupt.
*                                                                         If the output result blocks the next operation,
*                                                                         set bInstant to HI_TRUE.
*   Spec         :
*   Return Value: HI_SUCCESS: Success; Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2017-06-06
*          Author        :
*          Modification  : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_SVP_NNIE_Forward(SVP_NNIE_HANDLE *phSvpNnieHandle,
    const SVP_SRC_BLOB_S astSrc[], const SVP_NNIE_MODEL_S *pstModel,const SVP_DST_BLOB_S astDst[],
    const SVP_NNIE_FORWARD_CTRL_S *pstForwardCtrl,HI_BOOL bInstant){
HI_S32 result;
init_method_from_lib();
result = (*HI_MPI_SVP_NNIE_Forward_m)(phSvpNnieHandle,astSrc,pstModel,astDst,pstForwardCtrl,bInstant);
return result;
}

/*****************************************************************************
*   Prototype    : HI_MPI_SVP_NNIE_ForwardWithBbox
*   Description  : Perform CNN prediction on input sample(s), and output responses for corresponding sample(s)
*   Parameters   : SVP_NNIE_HANDLE                  *pNnieHandle        Returned handle ID of a task
*                  const SVP_SRC_BLOB_S                    astSrc[]           Input nodes' array.
*                  const SVP_SRC_BLOB_S                    astBbox[]          Input nodes' Bbox array.
*                  const SVP_NNIE_MODEL_S                  pstModel           CNN model data
*                  const SVP_DST_BLOB_S                    astDst[]           Output node array
*                  const SVP_NNIE_FORWARD_WITHBBOX_CTRL_S *pstForwardCtrl     Ctrl prameters
*                  HI_BOOL                           bInstant           Flag indicating whether to generate an interrupt.
*                                                                       If the output result blocks the next operation,
*                                                                       set bInstant to HI_TRUE.
*   Spec         :
*   Return Value: HI_SUCCESS: Success; Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2017-08-09
*          Author        :
*          Modification  : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_SVP_NNIE_ForwardWithBbox(SVP_NNIE_HANDLE *phSvpNnieHandle,
    const SVP_SRC_BLOB_S astSrc[], const SVP_SRC_BLOB_S astBbox[], const SVP_NNIE_MODEL_S *pstModel,
    const SVP_DST_BLOB_S astDst[], const SVP_NNIE_FORWARD_WITHBBOX_CTRL_S *pstForwardCtrl,HI_BOOL bInstant){
HI_S32 result;
init_method_from_lib();
result = (*HI_MPI_SVP_NNIE_ForwardWithBbox_m)(phSvpNnieHandle,astSrc,astBbox,pstModel,astDst,pstForwardCtrl,bInstant);
return result;
}

/*****************************************************************************
*   Prototype    : HI_MPI_SVP_NNIE_UnloadModel
*   Description  : Unload cnn model
*   Parameters   : SVP_NNIE_MODEL_S         *pstModel          Output model
*
*   Return Value : HI_SUCCESS: Success; Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2017-06-06
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
 HI_S32 HI_MPI_SVP_NNIE_UnloadModel(SVP_NNIE_MODEL_S *pstModel){
	HI_S32 result;
	
	init_method_from_lib();
	result = (*HI_MPI_SVP_NNIE_UnloadModel_m)(pstModel);
	return result;
}

/*****************************************************************************
*   Prototype    : HI_MPI_SVP_NNIE_Query
*   Description  : This API is used to query the status of a function runed on nnie.
                   In block mode, the system waits until the function that is being queried is called.
                   In non-block mode, the current status is queried and no action is taken.
*   Parameters   : SVP_NNIE_ID_E        enNnieId       NNIE Id
*                  SVP_NNIE_HANDLE      nnieHandle     nnieHandle of a called function. It is entered by users.
*                  HI_BOOL                    *pbFinish      Returned status
*                  HI_BOOL              bBlock         Flag indicating the block mode or non-block mode
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2017-06-06
*          Author        :
*          Modification  : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_SVP_NNIE_Query(SVP_NNIE_ID_E enNnieId,SVP_NNIE_HANDLE svpNnieHandle,HI_BOOL *pbFinish,HI_BOOL bBlock){
HI_S32 result;
init_method_from_lib();

result = (*HI_MPI_SVP_NNIE_Query_m)(enNnieId,svpNnieHandle,pbFinish,bBlock);
return result;
}


/*****************************************************************************
*   Prototype    : HI_MPI_SVP_NNIE_AddTskBuf
*   Description  : This API is used to record tskbuf in tskbuf array.
                   If the tskbuf has been recored, when user send the task with recored tskbuf,
                   the phyaddr will not be mapped again, this will improve the execution efficiency
*   Parameters   :
*                  const SVP_MEM_INFO_S* pstTskBuf    TskBuf
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2018-08-20
*          Author        :
*          Modification  : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_SVP_NNIE_AddTskBuf(const SVP_MEM_INFO_S* pstTskBuf){
	HI_S32 result;
	
	init_method_from_lib();
	result = (*HI_MPI_SVP_NNIE_AddTskBuf_m)(pstTskBuf);
	return result;
}


/*****************************************************************************
*   Prototype    : HI_MPI_SVP_NNIE_RemoveTskBuf
*   Description  : This API is used to remove tskbuf from tskBuf array and umap viraddr.
                   If user no longer uses the recored tskbuf, the recored tskbuf must be removed
*   Parameters   :
*                  const SVP_MEM_INFO_S* pstTskBuf    TskBuf
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2018-08-20
*          Author        :
*          Modification  : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_SVP_NNIE_RemoveTskBuf(const SVP_MEM_INFO_S* pstTskBuf){
	HI_S32 result;
	init_method_from_lib();
	result = (*HI_MPI_SVP_NNIE_RemoveTskBuf_m)(pstTskBuf);
	return result;
}


