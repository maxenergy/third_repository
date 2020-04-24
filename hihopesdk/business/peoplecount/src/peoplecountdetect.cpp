#include "peoplecountdetect.h"
#include <unistd.h>

PeopleCountDetect::PeopleCountDetect(std::string vendor, std::string algorithm) {
    mLoadCompleted = false;
    mObjectDetect = new ObjectDetect(vendor, algorithm);
    mDetectInterval = 0;
    init();
}

bool PeopleCountDetect::init() {
    mLoadCompleted = false;
    if (mObjectDetect == nullptr) {
        return false;
    }
    std::thread([&](){
        mObjectDetect->load();
        {
            std::unique_lock<std::mutex> locker(mMutex);
            mLoadCompleted = true;
            mCond.notify_all();
        }
    }).detach();
    return true;
}

bool PeopleCountDetect::readyToRun() {
    {
        std::unique_lock<std::mutex> locker(mMutex);
        if (!mLoadCompleted) {
            mCond.wait(locker, [&](){
                return mLoadCompleted;
            });
        }
    }
    {
        std::unique_lock<std::mutex> locker(mMutex);
        if (!mVideoCapture.isVaild()) {
            mCond.wait(locker, [&](){
                return mVideoCapture.isVaild();
            });
        }

    }
    return mObjectDetect->isValid();
}

bool PeopleCountDetect::loopOnce() {
    AIFrame frame;
    if (!mVideoCapture.read(frame.mFrameData)) {
        std::cout << "empty frame!!! skip" << std::endl;
        return true;
    };

    unsigned int count(0);
    ObjectDetectOutList aiDevOut;
    if (mObjectDetect->inference(frame, aiDevOut) != true) {
       std::cout << "detect failed!!! skip" << std::endl;
       return true;
    };

    for (auto iter = aiDevOut.cbegin(); iter != aiDevOut.cend(); iter++) {
        if (strcmp(iter->mName, "person") == 0) {
            count++;
        }
    }

    {
        std::lock_guard<std::mutex> locker(mMutex);
        mCurrentFrame = frame.mFrameData;
        //frame.copyTo(mCurrentFrame);
        //frame.release();
        mCurrentCount.mCount = count;
        mCurrentCount.mTimestamp =  static_cast<unsigned int>(time(nullptr));
    }

    //usleep(mDetectInterval*1000);
    return true;
}

void PeopleCountDetect::stop() {
    requestExitAndWait();
}

void PeopleCountDetect::start() {
    run();
}

bool PeopleCountDetect::setVideo(int videoIndex) {
    {
        std::unique_lock<std::mutex> locker(mMutex);
        mVideoCapture.open(videoIndex);
        mCond.notify_all();
    }
    return mVideoCapture.isVaild();
}

bool PeopleCountDetect::setVideo(std::string videoIndexURL) {
    {
        std::unique_lock<std::mutex> locker(mMutex);
        mVideoCapture.open(videoIndexURL);
        mCond.notify_all();
    }
    return mVideoCapture.isVaild();
}


bool PeopleCountDetect::setDetectInterval(unsigned int intervalMsec) {
    mDetectInterval = intervalMsec;
    return true;
}

void PeopleCountDetect::getCurretFrame(cv::Mat &image) {
    {
        std::lock_guard<std::mutex> locker(mMutex);
        if (mCurrentFrame.empty()) {
            return;
        }
        //mCurrentFrame.copyTo(image);
        image = mCurrentFrame;
    }
}

void PeopleCountDetect::getCurretPeopleCount(PeopleCount &peopleCount) {
    {
        std::lock_guard<std::mutex> locker(mMutex);
        peopleCount = mCurrentCount;

    }
}



