#ifndef NNIEYOLOV3_H
#define NNIEYOLOV3_H

#include "../../base/core/src/aidevicemanager.h"
#include "../../base/objectdetect/src/objectdetectinterface.h"

class PersonDetectNNIE :
        public AIDevice,
        public DynamicAIDevice<PersonDetectNNIE>,
        public ObjectDetectInterface {
public:
    PersonDetectNNIE();
    virtual bool load();
    virtual bool inference(AIFrame &frame, ObjectDetectOutList &outList);
};

#endif // NNIEYOLOV3_H
