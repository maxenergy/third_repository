#ifndef DUMMY_H
#define DUMMY_H

#include "core_public.h"

#define DummyNetImpl_Define(T1, T2) \
    class T1 : public T2 { \
    public:\
        virtual bool load(int device); \
        virtual bool detect(int device, Frame &frame, T2::Out &out); \
    private: \
        ReflectObject_SpecRegister_DECLARATION(T1); \
    }

DummyNetImpl_Define(MtcnnDummyImpl, MtcnnInterface);
DummyNetImpl_Define(FaceNeDummyImpl, FaceNetInterface);
DummyNetImpl_Define(ObjectDetectDummyImpl, ObjectDetectInterface);

#endif // DUMMY_H
