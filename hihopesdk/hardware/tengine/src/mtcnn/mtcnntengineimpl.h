#ifndef TENGINEMTCNN_H
#define TENGINEMTCNN_H

#include "mtcnn.hpp"
#include "mtcnn_utils.hpp"
#include "core_public.h"

class MtcnnTengineImpl : public MtcnnInterface {
public:
    MtcnnTengineImpl();
    virtual bool load(int device);
    virtual bool detect(int device, Frame &frame, MtcnnInterface::Out &out);

private:
    mtcnn *det;
    ReflectObject_SpecRegister_DECLARATION(MtcnnTengineImpl);
};

#endif // TENGINEMTCNN_H
