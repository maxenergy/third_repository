#include "dpucamera.h"
#include <opencv2/opencv.hpp>
#include <iostream>

#define PIC_WIDTH 1280
#define PIC_HEIGHT 720
/*RGB*/
#define PIXEL_WIDTH 3
#define PIC_SIZE PIC_WIDTH*PIC_HEIGHT*PIXEL_WIDTH

DpuCamera::DpuCamera() {
    l_id = 2;
    r_id = 3;
    mIsOpened = false;
}

DpuCamera::~DpuCamera() {
    release();
}

void DpuCamera::release() {
    if (mIsOpened) {
        dpu_stop(0);
        printf("stoped \n");
    }
}

bool DpuCamera::open() {
    if (mIsOpened) {
        return true;
    }
    if (dpu_init(l_id, r_id, 1) != 0) {
        std::cout << "[zlj] hi3559a dpu camera open failed" << std::endl;
        mIsOpened = false;
        return false;
    }
    mIsOpened = true;
    return true;
}

bool DpuCamera::open(int) {
    std::cout << "Unsupport. Will replace open()" << std::endl;
    return open();
}

bool DpuCamera::open(std::string) {
    std::cout << "Unsupport. Will replace open()" << std::endl;
    return open();
}

bool DpuCamera::read(cv::Mat &frame) {
    cv::Mat depthFrame;
    return read(frame, depthFrame);
}

bool DpuCamera::read(cv::Mat &rgbFrame, cv::Mat &) {
    if (mIsOpened == false) {
        return false;
    }

    char rgb_buf[PIC_SIZE];
    char depth_buf[PIC_SIZE];

    int ret = dpu_read(rgb_buf, depth_buf);
    if (ret != 0) {
        std::cout << "[zlj] WARNING!!! hi3559a dpu camera frame grabbed" << std::endl;
        return false;
    }

    rgbFrame = cv::Mat(PIC_HEIGHT, PIC_WIDTH, CV_8UC3, rgb_buf);
    return true;
}

bool DpuCamera::isVaild() {
    return mIsOpened;
}


