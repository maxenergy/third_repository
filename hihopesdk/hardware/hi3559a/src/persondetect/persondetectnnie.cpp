#include "persondetectnnie.h"

AIDEVICE_REGISTER(PersonDetectNNIE, "nnie_persondetect");

void yolv3nniealikInit();
void yolv3nniealikDetect(cv::Mat &image, ObjectDetectOutList &outList);

PersonDetectNNIE::PersonDetectNNIE() {
}

bool PersonDetectNNIE::load() {
    yolv3nniealikInit();
    return true;
}

bool PersonDetectNNIE::inference(AIFrame &frame, ObjectDetectOutList &outList) {
    if (frame.mFrameData.empty()) {
        return false;
    }

    yolv3nniealikDetect(frame.mFrameData, outList);
    return true;
}


