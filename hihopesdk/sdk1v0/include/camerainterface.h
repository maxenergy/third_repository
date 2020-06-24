#ifndef CAMERAINTERFACE_H
#define CAMERAINTERFACE_H

#include "reflectobject.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include "mppframe.h"
class CameraInterface : public ReflectObject {
public:
    virtual ~CameraInterface();

    virtual bool open();
    virtual bool open(int video);
    virtual bool open(std::string video);

    virtual bool read(cv::Mat &frame);
    virtual bool read(cv::Mat &rgbFrame, cv::Mat &depthFrame);
	virtual bool read(unsigned char *frame);
    virtual bool read(VIFrame &viFrame);
	virtual bool read(VIFrame &viFrame_brg,VIFrame &viFrame_ir);
	virtual bool read(int vpss_channel,VIFrame &viFrame);

    virtual unsigned int getWidth();
    virtual unsigned int getHeight();

    virtual bool isVaild();
    virtual void release();

protected:
    unsigned int mWidth = 0;
    unsigned int mHeigth = 0;
};

#endif // CAMERAINTERFACE_H
