#include "nniehelper.h"
#include "sample_nnie_ext.h"
#include "core_public.h"
#include <mutex>

#define NnieCoreNum 2
#define FaceNetNnieModelFile "models/facenet_inst.wk"

static int sNNIeStatus;
static std::mutex sMutex;
static OperatorInterface *sOperator;
static int sNnieCoreStatus[NnieCoreNum];

static SAMPLE_SVP_NNIE_CFG_S sFaceNetNnieCfg[NnieCoreNum];
static SAMPLE_SVP_NNIE_MODEL_S sFaceNetNnieModel[NnieCoreNum];
static SAMPLE_SVP_NNIE_PARAM_S sFaceNetNnieParam[NnieCoreNum];

int NnieTransMatToBuffer(cv::Mat mSrc, unsigned char *ppBuffer, int nWidth, int nHeight, int nBandNum, int nBPB) {
    std::vector<cv::Mat> channelrgb;
    split(mSrc, channelrgb);

     size_t nMemSize = nWidth * nHeight * nBandNum * nBPB;
    //这样可以改变外部*pBuffer的值
     memset(ppBuffer, 0, nMemSize);
     uchar *pT = ppBuffer;
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

int NnieUserCore(int coreIdx) {
    std::lock_guard<std::mutex> locker(sMutex);
    if (sNnieCoreStatus[coreIdx] == 1) {
        return -1;
    }
    sNnieCoreStatus[coreIdx] = 1;
    return 0;
}


int NNIEInit() {
    std::lock_guard<std::mutex> locker(sMutex);
    if(sNNIeStatus == 1) {
        return 0;
    }
    sOperator = OperatorFactory::createSingleInstance("opencl_operator");
    if (sOperator == nullptr) {
        return -1;
    }
    sNNIeStatus = 1;
    return 0;
}

int FaceNetNniePrewhiten(unsigned char *int_buf, unsigned int len, int *out_buf) {
    sOperator->preWhiten(int_buf, len, out_buf);
}

int FaceNetNnieLoadModel(int coreIdx) {
    if (NnieUserCore(coreIdx) != 0) {
        return -1;
    }
    sFaceNetNnieCfg[coreIdx].u32MaxRoiNum = 0;
    sFaceNetNnieCfg[coreIdx].u32MaxInputNum = 1;
    sFaceNetNnieCfg[coreIdx].aenNnieCoreId[0]= (SVP_NNIE_ID_E)coreIdx;
    NNIE_LOAD_MODEL(&sFaceNetNnieCfg[coreIdx], &sFaceNetNnieModel[coreIdx], &sFaceNetNnieParam[coreIdx], FaceNetNnieModelFile);
    return 0;
}

int FaceNetNnieLoadDetect(cv::Mat image_, int *out, int coreIdx) {
    if (image_.empty()) {
        return -1;
    }

    cv::Mat image;
    cv::resize(image_, image, cv::Size(160, 160), 0, 0, 3);

    unsigned char pix_buf[160*160*3];
    NnieTransMatToBuffer(image, pix_buf, 160, 160, 3, 1);

    int sent2nnie_buf[160*160*3];
    FaceNetNniePrewhiten(pix_buf, 160*160*3, sent2nnie_buf);
    NNIE_INFERENCE(sent2nnie_buf, &sFaceNetNnieCfg[coreIdx], &sFaceNetNnieParam[coreIdx]);

    nnie_out_buf buf;
    NNIE_GET_OUT_BUF(&sFaceNetNnieParam[coreIdx], &buf, 1);

    int *buf_temp = (int *)(buf.buf_addr[0]);

    //std::cout << "{";
    for(int i = 0;i<128;i++)
    {
        out[i] = buf_temp[i];
        //std::cout << buf_temp[i] << ",";
    }
    //std::cout << "}" << std::endl;
    return 0;
}
