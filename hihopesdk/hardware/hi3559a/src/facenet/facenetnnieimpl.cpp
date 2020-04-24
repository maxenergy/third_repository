#include "facenetnnieimpl.h"
#include "nniehelper.h"


ReflectObject_SpecRegister_IMPLEMENT(FaceNetNNIEImpl, AIInterface::getName("nnie" , "facenet"));

FaceNetNNIEImpl::FaceNetNNIEImpl() {

}

bool FaceNetNNIEImpl::load(int device) {
    if (NNIEInit() != 0) {
        return false;
    }
    if (FaceNetNnieLoadModel(device) != 0) {
        return false;
    };
    return true;
}

bool FaceNetNNIEImpl::detect(int device, Frame &frame, FaceNetInterface::Out &out) {
    if (frame.mFrameData.empty()) {
        std::cout << "nnie facenet input frame invalid" << std::endl;
        return false;
    }
    FaceNetOut box;
    if (FaceNetNnieLoadDetect(frame.mFrameData, box.mFeatureMap, device) == 0) {
        out.mOutList.push_back(box);
        return true;
    }
    return false;
}
