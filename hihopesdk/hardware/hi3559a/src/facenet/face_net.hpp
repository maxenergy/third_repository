#ifndef __FACE_NET_HPP__
#define __FACE_NET_HPP__

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
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <iostream>
#include <thread>
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

typedef struct Match_Ret_ {
    float best_match;
    int base_index;
} Match_Ret;

void opencl_init();
void opencl_deinit();

unsigned int Prewhiten(
        char *int_buf,
        unsigned int len,
        int *out_buf);

int MatchFeature(
        int *face,
        int *face_lib,
        int lib_size,
        Match_Ret *ret_buf);

extern "C" {

#define MODEL_NAME "facenet_inst.wk"
#define NNIE_MAX_OUT_BUF 10

typedef struct NNIE_OUT_BUF_ {
		void *buf_addr[NNIE_MAX_OUT_BUF];
		unsigned int stride[NNIE_MAX_OUT_BUF];
} nnie_out_buf;

void NNIE_LOAD_MODEL(
        SAMPLE_SVP_NNIE_CFG_S *stNnieCfg,
        SAMPLE_SVP_NNIE_MODEL_S *s_Model,
        SAMPLE_SVP_NNIE_PARAM_S *s_NnieParam,
        const char * pcModelName);

void NNIE_INFERENCE(
        void *data,
        SAMPLE_SVP_NNIE_CFG_S *stNnieCfg,
        SAMPLE_SVP_NNIE_PARAM_S *s_NnieParam);

void NNIE_GET_OUT_BUF(
        SAMPLE_SVP_NNIE_PARAM_S *pstNnieParam,
        nnie_out_buf *buf,
        int num);

void NNIE_Relese(
        SAMPLE_SVP_NNIE_MODEL_S *s_Model,
        SAMPLE_SVP_NNIE_PARAM_S *s_NnieParam);

}

class face_net
{
public:
		int feature_map_size;
		int nnie_id;
		int init_flag;
		face_net(const     char* model_dir,int id);
		~face_net();
		int get_feature_map(cv::Mat image,int *out_buf);
		int Match_Feature(int * face,int *face_lib,int lib_size,Match_Ret* out_buf);
private:
		SAMPLE_SVP_NNIE_MODEL_S s_Model;
		SAMPLE_SVP_NNIE_PARAM_S s_NnieParam;
		SAMPLE_SVP_NNIE_CFG_S	stNnieCfg;
protected:
	    int TransMatToBuffer(cv::Mat mSrc, char* ppBuffer, int nWidth, int nHeight, int nBandNum, int nBPB);
		double average_mean(char *x, int len);
		void prewhiten(char* inbuf,int *out_buf ,int len);
		double average_std(char *x, int len);
		double variance(char *x, int len);
};
#endif

