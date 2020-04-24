#ifndef PEOPLECOUNTDETECT_H
#define PEOPLECOUNTDETECT_H


#include "../../base/core/src/threadloop.h"
#include "../../base/core/src/camera.h"
#include "../../base/objectdetect/src/objectdetect.h"
#include "../../datamanager/db_peoplecount/src/peoplecount.h"
#include <opencv2/opencv.hpp>
#include <string>
#include <mutex>

class PeopleCountDetect : public ThreadLoop {
public:
    explicit PeopleCountDetect(std::string vendor, std::string algorithm);
    void stop();
    void start();
    bool setVideo(int videoIndex);
    bool setVideo(std::string videoIndexURL);
    bool setDetectInterval(unsigned int intervalMsec);
    void getCurretFrame(cv::Mat &image);
    void getCurretPeopleCount(PeopleCount &peopleCount);

private:
    bool init();
    virtual bool loopOnce();
    virtual bool readyToRun();

    bool mLoadCompleted;
    std::mutex mMutex;
    std::condition_variable mCond;

    ObjectDetect *mObjectDetect;
    Camera mVideoCapture;

    cv::Mat mCurrentFrame;
    PeopleCount mCurrentCount;

    unsigned int mDetectInterval;
 };

#endif // PEOPLECOUNTDETECT_H
