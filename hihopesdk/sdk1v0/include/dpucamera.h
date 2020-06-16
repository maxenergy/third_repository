#ifndef DPU_CAMERA_H
#define DPU_CAMERA_H

#include "core_public.h"
#include "../dpu/sample_dpu_main.h"
#include <atomic>

class DpuCamera :
        public CameraInterface,
        public ReflectObjectRegister<DpuCamera> {
public:
    DpuCamera();
    virtual ~DpuCamera();
    virtual bool open();
    virtual bool open(int video);
    virtual bool open(std::string video);

    virtual bool read(cv::Mat &frame);
    virtual bool read(cv::Mat &rgbFrame, cv::Mat &depthFrame);

    virtual void release();
    virtual bool isVaild();

private:
    int l_id;
    int r_id;
    std::atomic_bool mIsOpened;
};
#endif // DPU_CAMERA_H
