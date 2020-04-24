#ifndef MIPICAMERA_H
#define MIPICAMERA_H

#include "core_public.h"

class MipiCamera :
        public CameraInterface,
        public ReflectObjectRegister<MipiCamera> {
public:
    virtual bool open(int video);
    virtual bool read(cv::Mat &frame);
    virtual bool isVaild();

private:
    int mIdx;
};

#endif // MIPICAMERA_H
