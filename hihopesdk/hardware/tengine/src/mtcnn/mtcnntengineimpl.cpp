#include "mtcnntengineimpl.h"
#include "tenginehelper.h"

ReflectObject_SpecRegister_IMPLEMENT(MtcnnTengineImpl, AIInterface::getName("tengine" , "mtcnn"));

MtcnnTengineImpl::MtcnnTengineImpl() {
}

bool MtcnnTengineImpl::load(int) {
    if (tengineInit() != 0) {
        return false;
    }

    det = new mtcnn();
    det->load_3model("models");
    return true;
}

bool MtcnnTengineImpl::detect(int, Frame &frame,  MtcnnInterface::Out &out) {
    std::vector<face_box> faceList;
    det->detect(frame.mFrameData, faceList);
    std::for_each(faceList.cbegin(), faceList.cend(), [&](face_box box){
        MtcnnOut o;
        o.mScore = box.score;
        o.mRect = cv::Rect2f(cv::Point2f(box.x0, box.y0), cv::Point2f(box.x1, box.y1));
        for (int i = 0; i < 5; i++) {
            o.mKeyPoints.push_back(cv::Point2f(box.landmark.x[i], box.landmark.y[i]));
        }
        out.mOutList.push_back(o);
    });
    return true;
}
