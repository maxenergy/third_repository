#include "facedetectservice.h"
#include <unistd.h>

FaceDetectService::FaceDetectService(FaceDetect *faceRecognition, int maxAcquiredBufferCount,int maxDequeuedBufferCount) {
    mFaceRecognition = faceRecognition;
    if (mFaceRecognition == nullptr) {
        return;
    }

    mQueue = new FrameQueue(maxAcquiredBufferCount, maxDequeuedBufferCount);
    mConsumer = new FrameQueueConsumer(mQueue);
    mProducer = new FrameQueueProducer(mQueue);
}

FaceDetectService::~FaceDetectService() {
    stop();
}

void FaceDetectService::setPreviewCallBack(
        PreviewCallback callback) {
    mPreviewCallback = callback;
}

bool FaceDetectService::start() {
    if (!mCamera->isVaild()) {
        return false;
    }

    mCameraReadTask = new Task([&]()-> bool {
        cameraReadLoop();
        return true;
    });
    mCameraReadTask->start();

    mFaceDetectTask0 = new Task([&]()-> bool {
        faceDetectLoop(0);
        return true;
    });
    mFaceDetectTask0->start();

    mHandler = new Handler();
    return true;
}

void FaceDetectService::stop() {
    if (mCameraReadTask != nullptr) {
        mCameraReadTask->stop();
        free(mCameraReadTask);
    }

    if (mFaceDetectTask0 != nullptr) {
        mFaceDetectTask0->stop();
        free(mFaceDetectTask0);
    }

    if (mFaceDetectTask1 != nullptr) {
        mFaceDetectTask1->stop();
        free(mFaceDetectTask1);
    }

    if (mHandler != nullptr) {
        mHandler->stopSafty(true);
        free(mHandler);
    }
}

void FaceDetectService::cameraReadLoop() {
    Frame frame;
    
	if(mPipe.is_empty()){
#ifdef IR_CAMERA
    	mCamera_IR->read(frame.mRawdata,frame.IR_mRawdata);
#else
		mCamera->read(frame.mRawdata);
#endif
    	mPipe.push(frame);
	}
}


void FaceDetectService::faceDetectLoop(int device) {
    FaceDetect::Msg bob;
    bob.mFrame = mPipe.pop();
    bob.mFacenetDeviceCoreId = device;
    mFaceRecognition->detect(bob);
	bob.mFrame.mRawdata.release();
#ifdef IR_CAMERA
	bob.mFrame.IR_mRawdata.release();
#endif
    if (mPreviewCallback != nullptr) {
        mPreviewCallback(bob);
    }
}
