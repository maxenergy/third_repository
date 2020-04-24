#include "nnieyolov3.h"

void yolv3nniealikInit();
void yolv3nniealikDetect(cv::Mat &image, ObjectDetectOut &out);


int NNIEYolov3::init() {
    yolv3nniealikInit();
    return 0;
}

int NNIEYolov3::detect(cv::Mat &image, ObjectDetectOut &outs) {
    float ret_buf[100] = {0.0f};
    yolv3nniealikDetect(image, outs);;
    return 0;
}

std::string NNIEYolov3::descriptor() {
    return NNIEYolov3::getDescriptor();
}

std::string NNIEYolov3::getDescriptor() {
    return vendor() + "_" + algorithm();
}

std::string NNIEYolov3::algorithm() {
    return "yolov3";
}
