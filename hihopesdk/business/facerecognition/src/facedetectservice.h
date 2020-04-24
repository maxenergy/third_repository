#ifndef FACERECOGNITIONNETSERVICE_H
#define FACERECOGNITIONNETSERVICE_H

#include "core_public.h"
#include "facedetect.h"

class FaceDetectService
{
public:
    //typedef BufferQueue<FaceRecognitionNet::Bob, 3> Pipe;
    typedef std::function<void(FaceDetect::Msg bob)> PreviewCallback;
	typedef std::function<void(cv::Rect2f mRect)> PreviewCallback1;

    FaceDetectService(FaceDetect *faceRecognition, int maxAcquiredBufferCount = 2, int maxDequeuedBufferCount = 1);
    ~FaceDetectService();

    void stop();
    bool start();
    void setPreviewCallBack(PreviewCallback callback);
	//void setPreviewCallBack1(PreviewCallback1 callback);

    void cameraReadLoop();
    void faceDetectLoop(int device);

//    void mtcnnDetectLoop();
//    void facenetDetectLoop(int device);

//    Pipe<FaceRecognitionNet::Bob, 3> mPipe;
//    Task *mMtcnnDetectTask;
//    Task *mFacenetDetectTask0;
//    Task *mFacenetDetectTask1;

    Task *mCameraReadTask;
    Task *mFaceDetectTask0;
    Task *mFaceDetectTask1;

    Camera *mCamera;
	Camera *mCamera_IR;
    Handler *mHandler;
    FrameQueue *mQueue;
    FrameQueueConsumer *mConsumer;
    FrameQueueProducer *mProducer;
    FaceDetect *mFaceRecognition;

    PreviewCallback mPreviewCallback;
	//PreviewCallback1 mPreviewCallback1;
    Pool<FaceDetect::Msg> mOut;
    NoblockPipe<Frame, 3> mPipe;
};

#endif // FACERECOGNITIONNETSERVICE_H
