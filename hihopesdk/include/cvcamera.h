#ifndef CVVIDEOCAPTURE_H
#define CVVIDEOCAPTURE_H

#include "../reflect/reflectobject.h"
#include "../thread/threadloop.h"
#include "camerainterface.h"
#include <opencv2/opencv.hpp>
#include <iostream>


class CVCamera :
        public CameraInterface ,
        public ReflectObjectRegister<CVCamera> {
public:
    virtual bool open(int video);
    virtual bool open(std::string video);

    virtual bool read(cv::Mat &frame);
    virtual bool read(VIFrame &viFrame);
    virtual void release();
    virtual bool isVaild();

private:
    void dump();
    cv::VideoCapture mVideoCapture;
};



//class CVCamera1 : public CVCamera, public ThreadLoop {
//public:
//    CVCamera1();
//    virtual ~CVCamera1();
//    virtual void release();
//    virtual bool read(cv::Mat &frame);

//protected:
//    virtual bool init();
//    virtual bool loopOnce();

//private:
//    std::mutex mMutex;
//    cv::Mat mFont, mBack;
//    std::atomic_uint mIndex;
//};



//class CVCamera2 : public CVCamera, public ThreadLoop {
//public:
//    CVCamera2();
//    virtual ~CVCamera2();
//    virtual void release();
//    virtual bool read(cv::Mat &frame);

//protected:
//    virtual bool init();
//    virtual bool loopOnce();

//private:
//    std::mutex mMutex;
//    std::condition_variable mCond;
//    std::condition_variable mEmptyCond;
//    std::condition_variable mFullCond;
//    std::queue<cv::Mat> mFrameBuffer;
//    std::size_t mBufferMaxSize;
//};

#endif // CVVIDEOCAPTURE_H
