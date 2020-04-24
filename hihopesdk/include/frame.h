#ifndef AIFRAME_H
#define AIFRAME_H

#include <vector>
#include <functional>
#include <opencv2/opencv.hpp>
#include "mppframe.h"

class Frame {

public:
    cv::Mat mFrameData;
	VIFrame mRawdata;
	VIFrame IR_mRawdata;
};

typedef std::vector<Frame> AIFrameList;

#endif // AIFRAME_H
