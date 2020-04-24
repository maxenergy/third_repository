#include "objectdetectout.h"

#define IP_CAMERA 1
#if IP_CAMERA
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <iostream>
#include <thread>
#endif
#include "hi_type.h"

#define TEST_MODE 0
#define USE_YOLO_OR_TINYYOLO 1
#define  MAX_RET_COUNT  10
typedef struct retbuf
{
        int ret_count;
        char class_ret[MAX_RET_COUNT];
        int x0[MAX_RET_COUNT];
        int x1[MAX_RET_COUNT];
        int y0[MAX_RET_COUNT];
        int y1[MAX_RET_COUNT];
}DETECT_RET_BUF;

#define TDE2_COLOR_FMT_RGB888 6
#define TDE2_COLOR_FMT_ARGB8888 20

#define PIC_WIDTH 1920
#define PIC_HIGHT 1080
#define PIC_FMT TDE2_COLOR_FMT_ARGB8888
#define SCREEN_WIDTH 1920
#define SCREEN_HIGHT 1080

uchar detect_buf[2][416*416*3];
char buf_in_state=0;
const char *class_name[]={
            "background","person","bicycle","car","motorbike","aeroplane"
             ,"bus","train","truck","boat","traffic light",
             "fire hydrant","stop sign","parking meter","bench","bird",
             "cat","dog","horse","sheep","cow","elephant","bear","zebra","giraffe","backpack",
             "umbrella","handbag","tie","suitcase","frisbee"
            ,"kis","snowboard","sports ball"," kite","baseball bat"
            ,"aseball glove","skateboard","surfboard","tennis racket"," bottle"//42
            ,"ine glass","cup","fork"," knife","spoon"
            ,"owl","banana"," apple"," sandwich","orange "
            ,"roccoli","carrot"," hot dog","pizza","donut"
            ,"ake","chair","sofa"," pottedplant","bed"
            ,"iningtable","toilet","vmonitor"," laptop","mouse"
            ,"emote","keyboard"," cell phone"," microwave","oven"
            ,"oaster","sink","refrigerator"," book","clock "
            ,"ase","scissors"," teddy bear"," hair drier","toothbrush"
    };

extern "C" {
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


static SAMPLE_SVP_NNIE_MODEL_S s_stYolov3Model[2];
static SAMPLE_SVP_NNIE_PARAM_S s_stYolov3NnieParam[2];
#if USE_YOLO_OR_TINYYOLO
static SAMPLE_SVP_NNIE_YOLOV3_SOFTWARE_PARAM_S s_stYolov3SoftwareParam[2];
void deinit_yolov3(SAMPLE_SVP_NNIE_MODEL_S *s_stYolov3Model_ipc,
    SAMPLE_SVP_NNIE_PARAM_S *s_stYolov3NnieParam_ipc,SAMPLE_SVP_NNIE_YOLOV3_SOFTWARE_PARAM_S *s_stYolov3SoftwareParam_ipc);

void NNIE_Yolov3_HandleSig(SAMPLE_SVP_NNIE_MODEL_S *s_stYolov3Model_ipc,
    SAMPLE_SVP_NNIE_PARAM_S *s_stYolov3NnieParam_ipc,SAMPLE_SVP_NNIE_YOLOV3_SOFTWARE_PARAM_S *s_stYolov3SoftwareParam_ipc);

void NNIE_LOAD_MODEL_yolov3(SAMPLE_SVP_NNIE_CFG_S   *stNnieCfg_ipc,
    SAMPLE_SVP_NNIE_MODEL_S *s_stYolov3Model_ipc,SAMPLE_SVP_NNIE_PARAM_S *s_stYolov3NnieParam_ipc,SAMPLE_SVP_NNIE_YOLOV3_SOFTWARE_PARAM_S *s_stYolov3SoftwareParam_ipc);

void NNIE_process_yolov3(void *data,SAMPLE_SVP_NNIE_CFG_S *stNnieCfg_ipc,
    SAMPLE_SVP_NNIE_PARAM_S *s_stYolov3NnieParam_ipc,SAMPLE_SVP_NNIE_YOLOV3_SOFTWARE_PARAM_S *s_stYolov3SoftwareParam_ipc,DETECT_RET_BUF *buf_ret,int test_mode);

#else
static SAMPLE_SVP_NNIE_YOLOV3_TINY_SOFTWARE_PARAM_S s_stYolov3SoftwareParam[2];
void deinit_yolov3_tiny(SAMPLE_SVP_NNIE_MODEL_S *s_stYolov3Model_ipc,
    SAMPLE_SVP_NNIE_PARAM_S *s_stYolov3NnieParam_ipc,SAMPLE_SVP_NNIE_YOLOV3_TINY_SOFTWARE_PARAM_S *s_stYolov3SoftwareParam_ipc);

void NNIE_Yolov3_tiny_HandleSig(SAMPLE_SVP_NNIE_MODEL_S *s_stYolov3Model_ipc,
    SAMPLE_SVP_NNIE_PARAM_S *s_stYolov3NnieParam_ipc,SAMPLE_SVP_NNIE_YOLOV3_TINY_SOFTWARE_PARAM_S *s_stYolov3SoftwareParam_ipc);

void NNIE_LOAD_MODEL_yolov3_tiny(SAMPLE_SVP_NNIE_CFG_S  *stNnieCfg_ipc,
    SAMPLE_SVP_NNIE_MODEL_S *s_stYolov3Model_ipc,SAMPLE_SVP_NNIE_PARAM_S *s_stYolov3NnieParam_ipc,SAMPLE_SVP_NNIE_YOLOV3_TINY_SOFTWARE_PARAM_S *s_stYolov3SoftwareParam_ipc);

void NNIE_process_yolov3_tiny(void *data,SAMPLE_SVP_NNIE_CFG_S *stNnieCfg_ipc,
    SAMPLE_SVP_NNIE_PARAM_S *s_stYolov3NnieParam_ipc,SAMPLE_SVP_NNIE_YOLOV3_TINY_SOFTWARE_PARAM_S *s_stYolov3SoftwareParam_ipc,DETECT_RET_BUF *buf_ret,int test_mode);

#endif

    HI_S32 SAMPLE_SVP_NNIE_Detection_Result(SVP_BLOB_S *pstDstScore,
       SVP_BLOB_S *pstDstRoi, SVP_BLOB_S *pstClassRoiNum, HI_FLOAT f32PrintResultThresh,DETECT_RET_BUF * ret_buf);
}

#define USE_OPENCL 1

#if USE_OPENCL
void opencl_init(void);
void opencl_deinit();
unsigned int get_yolo_reslut(int **ret_buf,unsigned int *len,unsigned int *stride,float *af32Bias,unsigned int grid_num,SAMPLE_SVP_NNIE_YOLOV2_BBOX_S *out);
#endif
using namespace std;
cv::Mat alpha = cv::Mat::zeros(PIC_HIGHT, PIC_WIDTH, CV_8UC1);
DETECT_RET_BUF ret_buf;
static char **s_ppChCmdArgv = NULL;
SAMPLE_SVP_NNIE_CFG_S   stNnieCfg[2];
static int nnie_buf_state[2];
enum BUF_STATE{
    BUF_EMPTY =0,
    BUF_FILLED =1,
};

/******************************************************************************
* function : to process abnormal case
******************************************************************************/
void SAMPLE_SVP_HandleSig(int s32Signo)
{
    if (SIGINT == s32Signo || SIGTERM == s32Signo)
    {
        switch (*s_ppChCmdArgv[1])
        {
            case '0':
                {
#if USE_OPENCL

                  opencl_deinit();
#endif
#if USE_YOLO_OR_TINYYOLO
                    NNIE_Yolov3_HandleSig(&s_stYolov3Model[0],&s_stYolov3NnieParam[0],&s_stYolov3SoftwareParam[0]);
                    NNIE_Yolov3_HandleSig(&s_stYolov3Model[1],&s_stYolov3NnieParam[1],&s_stYolov3SoftwareParam[1]);
#else
                    NNIE_Yolov3_tiny_HandleSig(&s_stYolov3Model[0],&s_stYolov3NnieParam[0],&s_stYolov3SoftwareParam[0]);
                    NNIE_Yolov3_tiny_HandleSig(&s_stYolov3Model[1],&s_stYolov3NnieParam[1],&s_stYolov3SoftwareParam[1]);
#endif
                }
                break;
            default :
                {
                }
                break;
        }

        printf("\033[0;31mprogram termination abnormally!\033[0;39m\n");
    }
    exit(-1);
}
/******************************************************************************
* function : show usage
******************************************************************************/
void SAMPLE_SVP_Usage()
{
    printf("Usage -m: mode (0 ipc 1 uvc) (default mode is ipc)\n");
    printf("Usage -ac: account:passwd (default account is alik:123456)\n");
    printf("Usage -ad: ip addr (default ip is 192.168.1.107)\n");
    printf("Usage -p: port (default port 8081)\n");

}
int TransMatToBuffer(cv::Mat mSrc, uchar* ppBuffer, int nWidth, int nHeight, int nBandNum, int nBPB)
{
        vector<cv::Mat> channelrgb;
        split(mSrc, channelrgb);

         size_t nMemSize = nWidth * nHeight * nBandNum * nBPB;
        //这样可以改变外部*pBuffer的值
         memset(ppBuffer, 0, nMemSize);
         uchar* pT = ppBuffer;
         for(int i=nBandNum-1;i>=0;i--){
             for (int j = 0; j < nHeight; ++j)
             {
                unsigned char* data = channelrgb[i].ptr<unsigned char>(j);
                memcpy(pT, data, nWidth * nBPB);
                pT +=nWidth * nBPB;
            }
         }

    return 0;
}

#define IPC_MODE 0
#define UVC_MODE 1

int cam_mode = IPC_MODE;

unsigned int last_time;
unsigned int current_time;
#define NNIE_SKIP_FPS 2
/******************************************************************************
* function : ive sample
******************************************************************************/
void ipc_loop(char *video_addr)
{
    cv::namedWindow("Object Detection", cv::WINDOW_AUTOSIZE);
    //cv::setWindowProperty("Object Detection", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);


    cv::VideoCapture vcap;
    cv::VideoCapture vcap_usb(0);
    cv::Mat image;
    const std::string videoStreamAddress(video_addr); // = "http://alik:123456@192.168.1.107:8081";
    char *buf_show;
    int nnie_index = 0;

    int font_face = cv::FONT_HERSHEY_COMPLEX;
    double font_scale = 0.3;
    int thickness = 1;

    if(cam_mode == IPC_MODE){
        //open the video stream and make sure it's opened
        if(!vcap.open(videoStreamAddress)) {
            std::cout << "Error opening video stream or file" << std::endl;
            return;
        }
    }else{
        //vcap = new cv::VideoCapture(atoi(video_addr));
        //open the video stream and make sure it's opened
        if(!vcap_usb.isOpened()) {
            std::cout << "Error opening video stream or file" << std::endl;
            return;
        }

    }

    cv::Mat matFlipCameraImage;
    cv::Mat matFakeImage = cv::imread("./fakebg.png", cv::IMREAD_COLOR);
    if(!matFakeImage.data)
    {
        printf("No fakebg data \n");
        return;
    }
    printf("matFakeImage: (%d, %d) \n", matFakeImage.cols, matFakeImage.rows);

    cv::Mat matFakeImageROI = matFakeImage(cv::Rect(450, 0, 1470, 920));
    printf("matFakeImageROI: (%d, %d) \n", matFakeImageROI.cols, matFakeImageROI.rows);

    cv::Mat matCameraRegionMask = cv::Mat(cv::Size(1470, 920), CV_8UC1, cv::Scalar(1));
    printf("matCameraRegionMask: (%d, %d) \n", matCameraRegionMask.cols, matCameraRegionMask.rows);


    int test_fps=0;
    struct timeval start_time;
    struct timeval stop_time;
    int nnie_skip=0;

    while(1){

            if(cam_mode == IPC_MODE){
                    if(!vcap.read(matFlipCameraImage)){
                        std::cout << "IPC No frame" << std::endl;
                        cv::waitKey();
                        continue;
                    }
            }else{
                if(!vcap_usb.read(matFlipCameraImage)){
                        std::cout << "USB No frame" << std::endl;
                        cv::waitKey();
                        continue;
                    }
            }

           cv::flip(matFlipCameraImage, image, 0);

           cv::Mat dstImage;
           resize(image, dstImage, cv::Size(416, 416), 0, 0, 3);

            if(nnie_buf_state[nnie_index] == BUF_EMPTY){
              TransMatToBuffer(dstImage,detect_buf[nnie_index],416,416,3,1);
              nnie_buf_state[nnie_index] = BUF_FILLED;
              nnie_skip++;
              if(nnie_skip >=NNIE_SKIP_FPS){
                    nnie_skip=0;
                    nnie_index= nnie_index?0:1;
                }
              if(test_fps==0){
                  gettimeofday(&start_time, NULL);
                  test_fps++;
              }
              else if(test_fps==100){
                  gettimeofday(&stop_time, NULL);
                  test_fps = 0;
                  printf("100fps cost %ld ms \n",(stop_time.tv_sec-start_time.tv_sec)*1000+(stop_time.tv_usec-start_time.tv_usec)/10000);
              }
              else
                  test_fps++;
            }

            // for(int i =0;i<ret_buf.ret_count;i++)
            // {
            //  char socre_buf[20];
            //  cv::rectangle(dstImage,cvPoint(ret_buf.x0[i],ret_buf.y0[i]),cvPoint(ret_buf.x1[i],ret_buf.y1[i]),
            //  cv::Scalar(255,0,0),2,1,0);
            //  std::string text(class_name[ret_buf.class_ret[i]]);
            //  cv::putText(dstImage, text, cvPoint(ret_buf.x0[i],ret_buf.y0[i]-10), font_face, font_scale, cv::Scalar(0, 0, 255), thickness, 8, 0);
            // }


            for(int i =0;i<ret_buf.ret_count;i++)
            {
                char socre_buf[20];
                cv::rectangle(image,cvPoint(ret_buf.x0[i]*image.cols/416,ret_buf.y0[i]*image.rows/416),cvPoint(ret_buf.x1[i]*image.cols/416,ret_buf.y1[i]*image.rows/416),
                cv::Scalar(255,0,0),2,1,0);
                std::string text(class_name[ret_buf.class_ret[i]]);
                cv::putText(image, text, cvPoint(ret_buf.x0[i]*image.cols/416,ret_buf.y0[i]*image.rows/416-10), font_face, font_scale, cv::Scalar(0, 0, 255), thickness, 8, 0);
            }

            // for(int i =0;i<ret_buf.ret_count;i++)
            // {
            //  char socre_buf[20];
            //  cv::rectangle(image,cvPoint(ret_buf.x0[i],ret_buf.y0[i]),cvPoint(ret_buf.x1[i],ret_buf.y1[i]),
            //  cv::Scalar(255,0,0),2,1,0);
            //  std::string text(class_name[ret_buf.class_ret[i]]);
            //  cv::putText(image, text, cvPoint(ret_buf.x0[i],ret_buf.y0[i]-10), font_face, font_scale, cv::Scalar(0, 0, 255), thickness, 8, 0);
            //  printf("%s, %d, %d, %d, %d\n", text, ret_buf.x0[i], ret_buf.y0[i], ret_buf.x1[i], ret_buf.y1[i]);
            // }

            cv::Mat showImage;
            resize(image, showImage, cv::Size(1470, 920), 0, 0, 3);

            //showImage.copyTo(matFakeImageROI, matCameraRegionMask);

            cv::imshow("Object Detection", showImage);
            cv::waitKey(1);
    }

}



#define SAMPLE_SVP_NNIE_MAX(a,b)    (((a) > (b)) ? (a) : (b))
#define SAMPLE_SVP_NNIE_MIN(a,b)    (((a) < (b)) ? (a) : (b))

static void SVP_NNIE_Yolov1_Argswap(HI_S32* ps32Src1, HI_S32* ps32Src2,
    HI_U32 u32ArraySize)
{
    HI_U32 i = 0;
    HI_S32 s32Tmp = 0;
    for( i = 0; i < u32ArraySize; i++ )
    {
        s32Tmp = ps32Src1[i];
        ps32Src1[i] = ps32Src2[i];
        ps32Src2[i] = s32Tmp;
    }
}

static HI_S32 SVP_NNIE_Yolo_NonRecursiveArgQuickSort(HI_S32* ps32Array,
    HI_S32 s32Low, HI_S32 s32High, HI_U32 u32ArraySize,HI_U32 u32ScoreIdx,
    SAMPLE_SVP_NNIE_STACK_S *pstStack)
{
    HI_S32 i = s32Low;
    HI_S32 j = s32High;
    HI_S32 s32Top = 0;
    HI_S32 s32KeyConfidence = ps32Array[u32ArraySize * s32Low + u32ScoreIdx];
    pstStack[s32Top].s32Min = s32Low;
    pstStack[s32Top].s32Max = s32High;

    while(s32Top > -1)
    {
        s32Low = pstStack[s32Top].s32Min;
        s32High = pstStack[s32Top].s32Max;
        i = s32Low;
        j = s32High;
        s32Top--;

        s32KeyConfidence = ps32Array[u32ArraySize * s32Low + u32ScoreIdx];

        while(i < j)
        {
            while((i < j) && (s32KeyConfidence > ps32Array[j * u32ArraySize + u32ScoreIdx]))
            {
                j--;
            }
            if(i < j)
            {
                SVP_NNIE_Yolov1_Argswap(&ps32Array[i*u32ArraySize], &ps32Array[j*u32ArraySize],u32ArraySize);
                i++;
            }

            while((i < j) && (s32KeyConfidence < ps32Array[i*u32ArraySize + u32ScoreIdx]))
            {
                i++;
            }
            if(i < j)
            {
                SVP_NNIE_Yolov1_Argswap(&ps32Array[i*u32ArraySize], &ps32Array[j*u32ArraySize],u32ArraySize);
                j--;
            }
        }

        if(s32Low < i-1)
        {
            s32Top++;
            pstStack[s32Top].s32Min = s32Low;
            pstStack[s32Top].s32Max = i-1;
        }

        if(s32High > i+1)
        {
            s32Top++;
            pstStack[s32Top].s32Min = i+1;
            pstStack[s32Top].s32Max = s32High;
        }
    }
    return HI_SUCCESS;
}

static HI_DOUBLE SVP_NNIE_Yolov2_Iou(SAMPLE_SVP_NNIE_YOLOV2_BBOX_S *pstBbox1,
    SAMPLE_SVP_NNIE_YOLOV2_BBOX_S *pstBbox2)
{
    HI_FLOAT f32InterWidth = 0.0;
    HI_FLOAT f32InterHeight = 0.0;
    HI_DOUBLE f64InterArea = 0.0;
    HI_DOUBLE f64Box1Area = 0.0;
    HI_DOUBLE f64Box2Area = 0.0;
    HI_DOUBLE f64UnionArea = 0.0;

    f32InterWidth =  SAMPLE_SVP_NNIE_MIN(pstBbox1->f32Xmax, pstBbox2->f32Xmax) - SAMPLE_SVP_NNIE_MAX(pstBbox1->f32Xmin,pstBbox2->f32Xmin);
    f32InterHeight = SAMPLE_SVP_NNIE_MIN(pstBbox1->f32Ymax, pstBbox2->f32Ymax) - SAMPLE_SVP_NNIE_MAX(pstBbox1->f32Ymin,pstBbox2->f32Ymin);

    if(f32InterWidth <= 0 || f32InterHeight <= 0) return 0;

    f64InterArea = f32InterWidth * f32InterHeight;
    f64Box1Area = (pstBbox1->f32Xmax - pstBbox1->f32Xmin)* (pstBbox1->f32Ymax - pstBbox1->f32Ymin);
    f64Box2Area = (pstBbox2->f32Xmax - pstBbox2->f32Xmin)* (pstBbox2->f32Ymax - pstBbox2->f32Ymin);
    f64UnionArea = f64Box1Area + f64Box2Area - f64InterArea;

    return f64InterArea/f64UnionArea;
}


static HI_S32 SVP_NNIE_Yolov2_NonMaxSuppression( SAMPLE_SVP_NNIE_YOLOV2_BBOX_S* pstBbox,
    HI_U32 u32BboxNum, HI_U32 u32NmsThresh,HI_U32 u32MaxRoiNum)
{
    HI_U32 i,j;
    HI_U32 u32Num = 0;
    HI_DOUBLE f64Iou = 0.0;
    for (i = 0; i < u32BboxNum /*&& u32Num < u32MaxRoiNum*/; i++)
    {
        if(pstBbox[i].u32Mask == 0 )
        {
            for(j= i+1;j< u32BboxNum; j++)
            {
                if( pstBbox[j].u32Mask == 0 )
                {
                    f64Iou = SVP_NNIE_Yolov2_Iou(&pstBbox[i],&pstBbox[j]);
                    if(f64Iou >= (HI_DOUBLE)u32NmsThresh/SAMPLE_SVP_NNIE_QUANT_BASE)
                    {
                        pstBbox[j].u32Mask = 1;
                    }
                }
            }
        }
    }

    return HI_SUCCESS;
}

#define SAMPLE_SVP_NNIE_YOLOV3_REPORT_BLOB_NUM 3
#define SAMPLE_SVP_NNIE_YOLOV3_EACH_GRID_BIAS_NUM 6
static HI_S32 Hihope_Yolov3_GetResult_L(HI_S32 **pps32InputData,HI_U32 au32GridNumWidth[],
        HI_U32 au32GridNumHeight[],HI_U32 au32Stride[],HI_U32 u32EachGridBbox,HI_U32 u32ClassNum,HI_U32 u32SrcWidth,
        HI_U32 u32SrcHeight,HI_U32 u32MaxRoiNum,HI_U32 u32NmsThresh,HI_U32 u32ConfThresh,
        HI_FLOAT af32Bias[SAMPLE_SVP_NNIE_YOLOV3_REPORT_BLOB_NUM][SAMPLE_SVP_NNIE_YOLOV3_EACH_GRID_BIAS_NUM],
        HI_S32* ps32TmpBuf,HI_S32 *ps32DstScore, HI_S32 *ps32DstRoi, HI_S32 *ps32ClassRoiNum,HI_S32 Blob_num)
    {
        HI_S32 *ps32InputBlob = NULL;
        HI_FLOAT *pf32Permute = NULL;
        SAMPLE_SVP_NNIE_YOLOV3_BBOX_S *pstBbox = NULL;
        HI_S32 *ps32AssistBuf = NULL;
        HI_U32 u32TotalBboxNum = 0;
        HI_U32 u32ChnOffset = 0;
        HI_U32 u32HeightOffset = 0;
        HI_U32 u32BboxNum = 0;
        HI_U32 u32GridXIdx;
        HI_U32 u32GridYIdx;
        HI_U32 u32Offset;
        HI_FLOAT f32StartX;
        HI_FLOAT f32StartY;
        HI_FLOAT f32Width;
        HI_FLOAT f32Height;
        HI_FLOAT f32ObjScore;
        HI_U32 u32MaxValueIndex = 0;
        HI_FLOAT f32MaxScore;
        HI_S32 s32ClassScore;
        HI_U32 u32ClassRoiNum;
        HI_U32 i = 0, j = 0, k = 0, c = 0, h = 0, w = 0;
        HI_U32 u32BlobSize = 0;
        HI_U32 u32MaxBlobSize = 0;

        for(i = 0; i < Blob_num; i++)
        {
            u32BlobSize = au32GridNumWidth[i]*au32GridNumHeight[i]*sizeof(HI_U32)*
                SAMPLE_SVP_NNIE_YOLOV3_EACH_BBOX_INFER_RESULT_NUM*u32EachGridBbox;
            if(u32MaxBlobSize < u32BlobSize)
            {
                u32MaxBlobSize = u32BlobSize;
            }
        }

        for(i = 0; i < Blob_num; i++)
        {
            u32TotalBboxNum += au32GridNumWidth[i]*au32GridNumHeight[i]*u32EachGridBbox;
        }
        pf32Permute = (HI_FLOAT*)ps32TmpBuf;
        pstBbox = (SAMPLE_SVP_NNIE_YOLOV2_BBOX_S*)(pf32Permute+u32MaxBlobSize/sizeof(HI_S32));
        ps32AssistBuf = (HI_S32*)(pstBbox+u32TotalBboxNum);
        u32BboxNum = get_yolo_reslut(pps32InputData,au32GridNumWidth,au32Stride,(float *)af32Bias,Blob_num,pstBbox);
        (void)SVP_NNIE_Yolo_NonRecursiveArgQuickSort((HI_S32*)pstBbox, 0, u32BboxNum - 1,
            sizeof(SAMPLE_SVP_NNIE_YOLOV3_BBOX_S)/sizeof(HI_U32),4,(SAMPLE_SVP_NNIE_STACK_S*)ps32AssistBuf);

        (void)SVP_NNIE_Yolov2_NonMaxSuppression(pstBbox, u32BboxNum, u32NmsThresh, sizeof(SAMPLE_SVP_NNIE_YOLOV3_BBOX_S)/sizeof(HI_U32));
        //Get result
        for (i = 1; i < u32ClassNum; i++)
        {
            u32ClassRoiNum = 0;
            for(j = 0; j < u32BboxNum; j++)
            {
                if ((0 == pstBbox[j].u32Mask) && (i == pstBbox[j].u32ClassIdx) && (u32ClassRoiNum < u32MaxRoiNum))
                {
                    *(ps32DstRoi++) = SAMPLE_SVP_NNIE_MAX((HI_S32)(pstBbox[j].f32Xmin*u32SrcWidth), 0);
                    *(ps32DstRoi++) = SAMPLE_SVP_NNIE_MAX((HI_S32)(pstBbox[j].f32Ymin*u32SrcHeight), 0);
                    *(ps32DstRoi++) = SAMPLE_SVP_NNIE_MIN((HI_S32)(pstBbox[j].f32Xmax*u32SrcWidth), u32SrcWidth);
                    *(ps32DstRoi++) = SAMPLE_SVP_NNIE_MIN((HI_S32)(pstBbox[j].f32Ymax*u32SrcHeight), u32SrcHeight);
                    *(ps32DstScore++) = pstBbox[j].s32ClsScore;
                    u32ClassRoiNum++;
                }
            }
            *(ps32ClassRoiNum+i) = u32ClassRoiNum;
        }
        return HI_SUCCESS;
    }

static int Hihope_Yolov3_tiny_GetResult(SAMPLE_SVP_NNIE_PARAM_S*pstNnieParam,
    SAMPLE_SVP_NNIE_YOLOV3_TINY_SOFTWARE_PARAM_S* pstSoftwareParam,unsigned int grid_num)
{
    HI_U32 i = 0;
    HI_S32 *aps32InputBlob[3] = {0};
    HI_U32 au32Stride[3] = {0};

    for(i = 0; i < grid_num; i++)
    {
        aps32InputBlob[i] = (HI_S32*)pstNnieParam->astSegData[0].astDst[i].u64VirAddr;
        au32Stride[i] = pstNnieParam->astSegData[0].astDst[i].u32Stride;
    }

    return Hihope_Yolov3_GetResult_L(aps32InputBlob,pstSoftwareParam->au32GridNumWidth,
        pstSoftwareParam->au32GridNumHeight,au32Stride,pstSoftwareParam->u32BboxNumEachGrid,
        pstSoftwareParam->u32ClassNum,pstSoftwareParam->u32OriImWidth,
        pstSoftwareParam->u32OriImWidth,pstSoftwareParam->u32MaxRoiNum,pstSoftwareParam->u32NmsThresh,
        pstSoftwareParam->u32ConfThresh,pstSoftwareParam->af32Bias,
        (HI_S32*)pstSoftwareParam->stGetResultTmpBuf.u64VirAddr,
        (HI_S32*)pstSoftwareParam->stDstScore.u64VirAddr,
        (HI_S32*)pstSoftwareParam->stDstRoi.u64VirAddr,
        (HI_S32*)pstSoftwareParam->stClassRoiNum.u64VirAddr,2);
}


static int Hihope_Yolov3_GetResult(SAMPLE_SVP_NNIE_PARAM_S*pstNnieParam,
    SAMPLE_SVP_NNIE_YOLOV3_SOFTWARE_PARAM_S* pstSoftwareParam,unsigned int grid_num)
{
    HI_U32 i = 0;
    HI_S32 *aps32InputBlob[3] = {0};
    HI_U32 au32Stride[3] = {0};

    for(i = 0; i < grid_num; i++)
    {
        aps32InputBlob[i] = (HI_S32*)pstNnieParam->astSegData[0].astDst[i].u64VirAddr;
        au32Stride[i] = pstNnieParam->astSegData[0].astDst[i].u32Stride;
    }

    return Hihope_Yolov3_GetResult_L(aps32InputBlob,pstSoftwareParam->au32GridNumWidth,
        pstSoftwareParam->au32GridNumHeight,au32Stride,pstSoftwareParam->u32BboxNumEachGrid,
        pstSoftwareParam->u32ClassNum,pstSoftwareParam->u32OriImWidth,
        pstSoftwareParam->u32OriImWidth,pstSoftwareParam->u32MaxRoiNum,pstSoftwareParam->u32NmsThresh,
        pstSoftwareParam->u32ConfThresh,pstSoftwareParam->af32Bias,
        (HI_S32*)pstSoftwareParam->stGetResultTmpBuf.u64VirAddr,
        (HI_S32*)pstSoftwareParam->stDstScore.u64VirAddr,
        (HI_S32*)pstSoftwareParam->stDstRoi.u64VirAddr,
        (HI_S32*)pstSoftwareParam->stClassRoiNum.u64VirAddr,3);

}

#include <thread>
#include <mutex>
#include <condition_variable>

std::mutex mutex0_;
std::condition_variable cond0_;

std::mutex mutex00_;
std::condition_variable cond00_;


std::mutex mutex1_;
std::condition_variable cond1_;


std::mutex mutex11_;
std::condition_variable cond11_;

void nnie_loop(int ID)
{
  printf("nnie_loop[%d] start! \n",ID);
  while(1)
  {
      if (nnie_buf_state[ID] == BUF_EMPTY) {
          if (ID == 0) {
              std::unique_lock<std::mutex> locker(mutex0_);
              cond0_.wait(locker, [&]{return nnie_buf_state[ID] == BUF_FILLED;});
          }
          if (ID == 1) {
              std::unique_lock<std::mutex> locker(mutex1_);
              cond1_.wait(locker, [&]{return nnie_buf_state[ID] == BUF_FILLED;});
          }

      }
        if(nnie_buf_state[ID] == BUF_FILLED)
        {
            DETECT_RET_BUF ret_buf_temp;
#if USE_YOLO_OR_TINYYOLO
            NNIE_process_yolov3(detect_buf[ID],&stNnieCfg[ID],&s_stYolov3NnieParam[ID],&s_stYolov3SoftwareParam[ID],&ret_buf_temp,TEST_MODE);
#else
            NNIE_process_yolov3_tiny(detect_buf[ID],&stNnieCfg[ID],&s_stYolov3NnieParam[ID],&s_stYolov3SoftwareParam[ID],&ret_buf_temp,TEST_MODE);
#endif
            if(!TEST_MODE){
            #if USE_YOLO_OR_TINYYOLO
                Hihope_Yolov3_GetResult(&s_stYolov3NnieParam[ID],&s_stYolov3SoftwareParam[ID],3);
            #else
                Hihope_Yolov3_tiny_GetResult(&s_stYolov3NnieParam[ID],&s_stYolov3SoftwareParam[ID],2);
            #endif
                (void)SAMPLE_SVP_NNIE_Detection_Result(&s_stYolov3SoftwareParam[ID].stDstScore,
                    &s_stYolov3SoftwareParam[ID].stDstRoi, &s_stYolov3SoftwareParam[ID].stClassRoiNum,0.8,&ret_buf_temp);
            }
            ret_buf = ret_buf_temp;
            nnie_buf_state[ID] = BUF_EMPTY;
        }
  }

}


int test(int argc, char *argv[])
{
    int s32Ret = HI_SUCCESS;
    int count=0;
    char buf[255];
    char *default_account="admin:admin";
    char *default_port="8081";
    char *default_addr="192.168.1.104";
    char* addr =NULL;
    char *account = NULL;
    char *port = NULL;
    if(argc < 2)
        SAMPLE_SVP_Usage();

    for(int i = 1;i<argc;i++)
    {
        if (strcmp("-m", argv[i]) == 0){
            cam_mode = atoi(argv[i+1]);
            printf("mode is %d\n",cam_mode);
            continue;
        }
        if (strcmp("-ad", argv[i]) == 0){
            addr = argv[i+1];
            printf("addr is %s\n",addr);
            continue;
        }
        if (strcmp("-ac", argv[i]) == 0){
            account = argv[i+1];
            printf("account is %s\n",account);
            continue;
        }

        if (strcmp("-p", argv[i]) == 0){
            port = argv[i+1];
            printf("port is %s\n",port);
            continue;
        }
    }

    if(account == NULL)
        account = default_account;

    if(port == NULL)
        port = default_port;

    if(addr == NULL)
        addr = default_addr;

    if(cam_mode == IPC_MODE)
        sprintf(buf,"http://%s@%s:%s/video",account,addr,port);
    else
      sprintf(buf,"%s",addr);

    printf("video addr is :%s \n",buf);
    signal(SIGINT, SAMPLE_SVP_HandleSig);
    signal(SIGTERM, SAMPLE_SVP_HandleSig);

    stNnieCfg[0].aenNnieCoreId[0]=SVP_NNIE_ID_0;
    stNnieCfg[0].u32MaxInputNum =1;

    stNnieCfg[1].aenNnieCoreId[0]=SVP_NNIE_ID_1;
    stNnieCfg[1].u32MaxInputNum =1;

     //添加透明度通道
    for (int i = 0; i < PIC_HIGHT; i++)
    {
        for (int j = 0; j < PIC_WIDTH; j++)
        {
            alpha.at<uchar>(i, j) = 255;
        }
    }

#if USE_YOLO_OR_TINYYOLO
    NNIE_LOAD_MODEL_yolov3(&stNnieCfg[0],&s_stYolov3Model[0],&s_stYolov3NnieParam[0],&s_stYolov3SoftwareParam[0]);
    NNIE_LOAD_MODEL_yolov3(&stNnieCfg[1],&s_stYolov3Model[1],&s_stYolov3NnieParam[1],&s_stYolov3SoftwareParam[1]);
#else
    NNIE_LOAD_MODEL_yolov3_tiny(&stNnieCfg[0],&s_stYolov3Model[0],&s_stYolov3NnieParam[0],&s_stYolov3SoftwareParam[0]);
    NNIE_LOAD_MODEL_yolov3_tiny(&stNnieCfg[1],&s_stYolov3Model[1],&s_stYolov3NnieParam[1],&s_stYolov3SoftwareParam[1]);
#endif
#if USE_OPENCL
    opencl_init();
#endif
    std::thread t0(nnie_loop,0);
    t0.detach();
    std::thread t1(nnie_loop,1);
    t1.detach();
    ipc_loop(buf);
}

void yolv3nniealikInit()
{
    stNnieCfg[0].aenNnieCoreId[0]=SVP_NNIE_ID_0;
    stNnieCfg[0].u32MaxInputNum =1;

    stNnieCfg[1].aenNnieCoreId[0]=SVP_NNIE_ID_1;
    stNnieCfg[1].u32MaxInputNum =1;

     //添加透明度通道
    for (int i = 0; i < PIC_HIGHT; i++)
    {
        for (int j = 0; j < PIC_WIDTH; j++)
        {
            alpha.at<uchar>(i, j) = 255;
        }
    }

#if USE_YOLO_OR_TINYYOLO
    NNIE_LOAD_MODEL_yolov3(&stNnieCfg[0],&s_stYolov3Model[0],&s_stYolov3NnieParam[0],&s_stYolov3SoftwareParam[0]);
    NNIE_LOAD_MODEL_yolov3(&stNnieCfg[1],&s_stYolov3Model[1],&s_stYolov3NnieParam[1],&s_stYolov3SoftwareParam[1]);
#else
    NNIE_LOAD_MODEL_yolov3_tiny(&stNnieCfg[0],&s_stYolov3Model[0],&s_stYolov3NnieParam[0],&s_stYolov3SoftwareParam[0]);
    NNIE_LOAD_MODEL_yolov3_tiny(&stNnieCfg[1],&s_stYolov3Model[1],&s_stYolov3NnieParam[1],&s_stYolov3SoftwareParam[1]);
#endif
#if USE_OPENCL
    opencl_init();
#endif

    std::thread t0(nnie_loop,0);
    t0.detach();
    std::thread t1(nnie_loop,1);
    t1.detach();
}

int nnie_index = 0;
int test_fps=0;
struct timeval start_time;
struct timeval stop_time;
int nnie_skip=0;
int font_face = cv::FONT_HERSHEY_COMPLEX;
double font_scale = 0.3;
int thickness = 1;

void yolv3nniealikProcess(cv::Mat &image) {

     if(nnie_buf_state[nnie_index] == BUF_EMPTY){
      cv::Mat dstImage;
      resize(image, dstImage, cv::Size(416, 416), 0, 0, 3);

       if (nnie_index == 0){
           std::unique_lock<std::mutex> locker(mutex0_);
           TransMatToBuffer(dstImage,detect_buf[nnie_index],416,416,3,1);
           nnie_buf_state[nnie_index] = BUF_FILLED;
           cond0_.notify_one();
       }

       if (nnie_index == 1) {
           std::unique_lock<std::mutex> locker1(mutex1_);
           TransMatToBuffer(dstImage,detect_buf[nnie_index],416,416,3,1);
           nnie_buf_state[nnie_index] = BUF_FILLED;
           cond1_.notify_one();
       }
       nnie_skip++;
       if(nnie_skip >=NNIE_SKIP_FPS){
             nnie_skip=0;
             nnie_index= nnie_index?0:1;
         }
       if(test_fps==0){
           gettimeofday(&start_time, NULL);
           test_fps++;
       }
       else if(test_fps==100){
           gettimeofday(&stop_time, NULL);
           test_fps = 0;
           printf("100fps cost %ld ms \n",(stop_time.tv_sec-start_time.tv_sec)*1000+(stop_time.tv_usec-start_time.tv_usec)/10000);
       }
       else
           test_fps++;
     }

     for(int i =0;i<ret_buf.ret_count;i++)
     {
         char socre_buf[20];
         cv::rectangle(image,cvPoint(ret_buf.x0[i]*image.cols/416,ret_buf.y0[i]*image.rows/416),cvPoint(ret_buf.x1[i]*image.cols/416,ret_buf.y1[i]*image.rows/416),
         cv::Scalar(255,0,0),2,1,0);
         std::string text(class_name[ret_buf.class_ret[i]]);
         cv::putText(image, text, cvPoint(ret_buf.x0[i]*image.cols/416,ret_buf.y0[i]*image.rows/416-10), font_face, font_scale, cv::Scalar(0, 0, 255), thickness, 8, 0);
     }
}

void yolv3nniealikDetect(cv::Mat &image, ObjectDetectOut &out) {
    yolv3nniealikProcess(image);
    //std::cout << "detect objects count:" << ret_buf.ret_count << std::endl;
    for(int i =0; i<ret_buf.ret_count; i++){
        //std::cout << class_name[ret_buf.class_ret[i]] << ":" << ret_buf.x0[i] << "," << ret_buf.x1[i] << "," << ret_buf.y0[i] << "," << ret_buf.y1[i] << std::endl;
        ObjectDetectOutItem item;
        sprintf(item.mName, "%s",  class_name[ret_buf.class_ret[i]]);
        item.mBox[0] = ret_buf.x0[i];
        item.mBox[1] = ret_buf.x1[i];
        item.mBox[2] = ret_buf.y0[i];
        item.mBox[3] = ret_buf.y1[i];
        out.push_back(item);
    }
    return;
}
