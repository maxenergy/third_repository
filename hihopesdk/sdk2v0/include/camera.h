#ifndef CAMERA_H
#define CAMERA_H

#include "camerainterface.h"
#include <memory>
#include "mppframe.h"

class Camera {
public:
    Camera(std::string type);

    bool open();
    bool open(int video);
    bool open(std::string video);
    bool read(cv::Mat &frame);
    bool read(cv::Mat &rgbFrame, cv::Mat &depthFrame);
	bool read(unsigned char *frame);
	bool read(VIFrame &viFrame);
	bool read(VIFrame &viFrame_bgr,VIFrame &viFrame_ir);
	bool read(int vpss_channel,VIFrame &viFrame);

    bool isVaild();
    void release();

    unsigned int getWidth();
    unsigned int getHeight();

private:
    int mCnt;
    double mCost;
    time_t mCurTime;
    time_t mLastTime;
    CameraInterface *mImpl;
};

#endif // CAMERA_H
