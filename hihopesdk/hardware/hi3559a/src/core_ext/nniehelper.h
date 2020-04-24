#ifndef NNIEHELPER_H
#define NNIEHELPER_H

#include "opencv2/opencv.hpp"

int NNIEInit();
int FaceNetNnieLoadModel(int coreIdx);
int FaceNetNnieLoadDetect(cv::Mat image_, int *out, int coreIdx);

#endif // NNIEHELPER_H
