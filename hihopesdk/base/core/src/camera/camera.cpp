#include "camera.h"
#include "cvcamera.h"
#include "mipicamera.h"


Camera::Camera(std::string type) {
    CVCamera();
	MipiCamera();
    mImpl = ReflectObjectClass::createObject<CameraInterface>(type);
    if (mImpl == nullptr) {
        return;
    }

    mCnt = 0;
    mCost = 0;
    mLastTime = time(nullptr);
}

bool Camera::open() {
    if (mImpl == nullptr) {
        return false;
    }
    mImpl->release();
    if (!mImpl->open()) {
        std::cout << "[zlj] ERROR!!! camera open failed" << std::endl;
        return false;
    }
    return true;
}

bool Camera::open(int video) {
    if (mImpl == nullptr) {
        return false;
    }
    mImpl->release();
    if (!mImpl->open(video)) {
        std::cout << "[zlj] ERROR!!! camera open failed" << std::endl;
        return false;
    }
    return true;
}

bool Camera::open(std::string video) {
    if (mImpl == nullptr) {
        return false;
    }
    mImpl->release();
    if (!mImpl->open(video)) {
        std::cout << "[zlj] ERROR!!! camera open failed" << std::endl;
        return false;
    }
    return true;
}

bool Camera::read(cv::Mat &frame) {
    if (mImpl == nullptr) {
        return false;
    }
    clock_t start = clock();
    bool ret = mImpl->read(frame);
    if (!ret) {
        std::cout << "[zlj] WARNING!!! blank frame grabbed" << std::endl;
    }
    clock_t end = clock();
    std::cout << "[zlj] camera read frame cost time:" << (end-start)/1000.0f << std::endl;

    mCnt++;
    mCost += end-start;
    if (mCnt >= 100) {
        mCurTime = time(nullptr);
        std::cout << "camera fps: " << mCnt/(mCurTime-mLastTime) << std::endl;
        //std::cout << "camera clock fps: " << mCnt/(mCost/CLOCKS_PER_SEC) << std::endl;
        mCnt = 0;
        mCost = 0;
        mLastTime = mCurTime;
    }
    return ret;
}

bool Camera::read(unsigned char *frame) {
    if (mImpl == nullptr) {
        return false;
    }
    bool ret = mImpl->read(frame);
    if (!ret) {
        std::cout << "[zlj] WARNING!!! blank frame grabbed" << std::endl;
    }
#if 0
    mCnt++;
    mCost += end-start;
    if (mCnt >= 100) {
        mCurTime = time(nullptr);
        std::cout << "camera fps: " << mCnt/(mCurTime-mLastTime) << std::endl;
        //std::cout << "camera clock fps: " << mCnt/(mCost/CLOCKS_PER_SEC) << std::endl;
        mCnt = 0;
        mCost = 0;
        mLastTime = mCurTime;
    }
#endif
    return ret;
}

bool Camera::read(VIFrame &frame) {
    if (mImpl == nullptr) {
        return false;
    }
    bool ret = mImpl->read(frame);
    if (!ret) {
        std::cout << "[zlj] WARNING!!! blank frame grabbed" << std::endl;
    }
#if 0
    mCnt++;
    if (mCnt >= 100) {
        mCurTime = time(nullptr);
        std::cout << "camera fps: " << mCnt/(mCurTime-mLastTime) << std::endl;
        mCnt = 0;
        mCost = 0;
        mLastTime = mCurTime;
    }
#endif
    return ret;
}


bool Camera::read(VIFrame &viFrame_bgr,VIFrame &viFrame_ir){
    if (mImpl == nullptr) {
        return false;
    }
    bool ret = mImpl->read(viFrame_bgr,viFrame_ir);
    if (!ret) {
        std::cout << "[zlj] WARNING!!! blank frame grabbed" << std::endl;
    }
#if 1
    mCnt++;
    if (mCnt >= 100) {
        mCurTime = time(nullptr);
        std::cout << "camera fps: " << mCnt/(mCurTime-mLastTime) << std::endl;
        mCnt = 0;
        mCost = 0;
        mLastTime = mCurTime;
    }
#endif
    return ret;
}



bool Camera::read(cv::Mat &rgbFrame, cv::Mat &depthFrame) {
    if (mImpl == nullptr) {
        return false;
    }
    clock_t start = clock();
    bool ret = mImpl->read(rgbFrame, depthFrame);
    if (!ret) {
        std::cout << "[zlj] WARNING!!! blank frame grabbed" << std::endl;
    }
    clock_t end = clock();
    //std::cout << "[zlj] camera read frame cost time:" << (end-start)/1000.0f << std::endl;

    mCnt++;
    mCost += end-start;
    if (mCnt >= 100) {
        mCurTime = time(nullptr);
        std::cout << "camera fps: " << mCnt/(mCurTime-mLastTime) << std::endl;
        std::cout << "camera clock fps: " << mCnt/(mCost/CLOCKS_PER_SEC) << std::endl;
        mCnt = 0;
        mCost = 0;
        mLastTime = mCurTime;
    }
    return ret;
}

void Camera::release() {
    if (mImpl == nullptr) {
        return;
    }
    mImpl->release();
}

bool Camera::isVaild() {
    if (mImpl == nullptr) {
        return false;
    }
    if (mImpl->isVaild()) {
        return true;
    } else {
        std::cout << "[zlj] camera is invalid" << std::endl;
        return false;
    }
}

unsigned int Camera::getWidth() {
    return mImpl->getWidth();
}

unsigned int Camera::getHeight() {
    return mImpl->getHeight();
}
