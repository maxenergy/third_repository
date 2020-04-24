#ifndef NNIEYOLOV3_H
#define NNIEYOLOV3_H

#include "nnie.h"
#include <objectdetectdevice.h>
#include <aidevicemanager.h>

class NNIEYolov3 : public ObjectDetect, public NNIE, public DynamicAIDevice<NNIEYolov3> {
public:
    virtual int init();
    virtual int detect(cv::Mat &image, ObjectDetectOut &outs);
    virtual std::string descriptor();
    static std::string getDescriptor();
    static std::string algorithm();
};

#endif // NNIEYOLOV3_H
