#ifndef FACERECOGNITIONNET_H
#define FACERECOGNITIONNET_H

#include "core_public.h"
#include <functional>

class FaceDetect
{
public:
    struct Msg {
        Frame mFrame;
        MtcnnInterface::Out mMtcnnInterfaceOut;
        MtcnnInterface::Out mMtcnnInterfaceOutOK;
        FaceNetInterface::Out mFaceNetInterfaceOut;
        int mMtcnnDeviceCoreId;
        int mFacenetDeviceCoreId;
    };

    FaceDetect();
	~FaceDetect();
    bool init();
    bool isValid();
    bool detect(Msg &msg);
    bool mtcnnDetect(Msg &msg);
    bool facenetDetect(Msg &msg);

    void drawRectangle(Msg &msg);

private:
    int mCnt;
    double mCost;
    time_t mCurTime;
    time_t mLastTime;

    MtcnnService *mMtcnn;
    FaceNetService *mFaceNets[2];

    float mScale = 1;
    bool mFilterMaxFaceEnable;
};

#endif // FACERECOGNITIONNET_H
