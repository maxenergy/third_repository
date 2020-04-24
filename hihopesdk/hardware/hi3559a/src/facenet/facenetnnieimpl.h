#ifndef FACENETNNIEIMPL_H
#define FACENETNNIEIMPL_H

#include "core_public.h"

class FaceNetNNIEImpl : public FaceNetInterface {
public:
    FaceNetNNIEImpl();
    virtual bool load(int device);
    virtual bool detect(int device, Frame &frame, FaceNetInterface::Out &out);

private:
    ReflectObject_SpecRegister_DECLARATION(FaceNetNNIEImpl);
};

#endif // FACENETNNIEIMPL_H
