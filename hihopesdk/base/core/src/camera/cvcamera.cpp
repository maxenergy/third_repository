#include "cvcamera.h"
#include "../thread/threadloop.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <unistd.h>

#define FK_CAM_TEST
void CVCamera::dump() {
#ifdef FK_CAM_TEST
	return;
#else
    if (!mVideoCapture.isOpened()) {
        return ;
    }

    mWidth = static_cast<unsigned int>(mVideoCapture.get(CV_CAP_PROP_FRAME_WIDTH));
    mHeigth = static_cast<unsigned int>(mVideoCapture.get(CV_CAP_PROP_FRAME_HEIGHT));

    std::cout << "[zlj] ip video stream info: "
       <<  "rate=" << mVideoCapture.get(CV_CAP_PROP_FPS) << ", "
       <<  "width=" << mVideoCapture.get(CV_CAP_PROP_FRAME_WIDTH) << ", "
       <<  "height=" << mVideoCapture.get(CV_CAP_PROP_FRAME_HEIGHT) << std::endl;
#endif
}

bool CVCamera::open(int video) {
#ifdef FK_CAM_TEST
	 std::cout << "[cvcamera fk] open video success"<< video << std::endl;
	 return true;
#else	
    release();
     if (!mVideoCapture.open(video)) {
         std::cout << "[zlj] ERROR!!! open video device failed "<< video << std::endl;
         return false;
     }
     std::cout << "[cvcamera] open video success"<< video << std::endl;
     dump();
     return true;
#endif
}

bool CVCamera::open(std::string video) {
#ifdef FK_CAM_TEST
		 std::cout << "[cvcamera fk] open video success"<< video << std::endl;
		 return true;
#else	
    release();
    if (!mVideoCapture.open(video)) {
        std::cout << "[cvcamera] ERROR!!! open video device failed "<< video << std::endl;
        return false;
    }
    std::cout << "[cvcamera] open video success"<< video << std::endl;
    dump();
    return true;
#endif
}
bool CVCamera::read(VIFrame &viFrame){
	return true;
}

bool CVCamera::read(cv::Mat &frame) {
#ifdef FK_CAM_TEST
		  usleep(10*1000);
          frame = cv::imread(std::string("fk_cam.jpg"));
          return true;
#else	
    return mVideoCapture.read(frame);
#endif
}

bool CVCamera::isVaild() {
#ifdef FK_CAM_TEST
	 return true;
#else	
    return mVideoCapture.isOpened();
#endif
}

void CVCamera::release() {
#ifdef FK_CAM_TEST
		 return;
#else
    if (mVideoCapture.isOpened()) {
        mVideoCapture.release();
    }
#endif
}

//CVCamera1::CVCamera1() {
//    //mFrames.fill(cv::Mat());
//    //mFrames[0] = cv::Mat();
//    //mFrames[1] = cv::Mat();
//}

//CVCamera1::~CVCamera1(){

//}

//void CVCamera1::release() {
//    requestExitAndWait();
//    CVCamera::release();
//}

//bool CVCamera1::init() {
//    if (CVCamera::init()) {
//        run();
//        return true;
//    }
//    return false;
//}

//bool CVCamera1::read(cv::Mat &frame) {
//    if (mIndex == 0) {
//        frame = mBack;
//    } else {
//        frame = mFont;
//    }
//    return !frame.empty();
//}

//bool CVCamera1::loopOnce() {
//    if (mIndex == 0) {
//        CVCamera::read(mFont);
//        mIndex = 1;
//    } else {
//        CVCamera::read(mBack);
//        mIndex = 0;
//    }
//    return true;
//}



//CVCamera2::CVCamera2() {
//}

//CVCamera2::~CVCamera2(){

//}

//void CVCamera2::release() {
//    requestExitAndWait();
//    CVCamera::release();
//}

//bool CVCamera2::init() {
//    if (CVCamera::init()) {
//        run();
//        return true;
//    }
//    return false;
//}

//bool CVCamera2::read(cv::Mat &frame) {
//    {
//        std::unique_lock<std::mutex> locker(mMutex);
//        if (mFrameBuffer.empty()) {
//            mCond.wait(locker, [&](){
//                return !mFrameBuffer.empty();
//            });
//        }

//        frame = mFrameBuffer.front();
//        mFrameBuffer.pop();
//        mCond.notify_all();
//    }
//    return frame.empty()?1:0;
//}

//bool CVCamera2::loopOnce() {
//    cv::Mat frame;
//    {
//        std::unique_lock<std::mutex> locker(mMutex);
//        if (mFrameBuffer.size() >= mBufferMaxSize) {
//            mCond.wait(locker, [&](){
//                return mFrameBuffer.size() <= mBufferMaxSize/2;
//            });
//        }
//    }
//    CVCamera::read(frame);
//    {
//        std::unique_lock<std::mutex> locker(mMutex);
//        mFrameBuffer.push(frame);
//        mCond.notify_all();
//    }
//    return true;
//}

