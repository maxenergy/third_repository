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

void FaceDetectService::setRecognitionCallBack(
        PreviewCallback callback) {
    mRecognitionCallback = callback;
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

    mFaceDetectTask = new Task([&]()-> bool {
        faceDetectLoop(0);
        return true;
    });
    mFaceDetectTask->start();

	mFaceRecognitionTask = new Task([&]()-> bool {
        faceRecognitionLoop(0);
        return true;
    });
    mFaceRecognitionTask->start();


    mHandler = new Handler();
    return true;
}

void FaceDetectService::stop() {
    if (mCameraReadTask != nullptr) {
        mCameraReadTask->stop();
        free(mCameraReadTask);
    }

    if (mFaceDetectTask != nullptr) {
        mFaceDetectTask->stop();
        free(mFaceDetectTask);
    }

    if (mFaceRecognitionTask != nullptr) {
        mFaceRecognitionTask->stop();
        free(mFaceRecognitionTask);
    }

    if (mHandler != nullptr) {
        mHandler->stopSafty(true);
        free(mHandler);
    }
}

void FaceDetectService::cameraReadLoop() {
    Frame frame;
#ifdef BUILD_FACTORY_TEST_APP
	usleep(1000*10);
#endif
	if(mPipe.size() <= 2){
#ifdef IR_CAMERA
    	mCamera_IR->read(frame.mRawdata,frame.IR_mRawdata);
#else
		mCamera->read(frame.mRawdata);
#endif
		if(mPipe_frg.size() <= 10){
    		mPipe.push(frame);
		}else{
			frame.mRawdata.release();
#ifdef IR_CAMERA
			frame.IR_mRawdata.release();
#endif
		}
	}
}


void FaceDetectService::trigger_frg(MtcnnInterface::Out mtcnn_out)
{
		mPipe_frg.push(mtcnn_out);
}

void FaceDetectService::faceDetectLoop(int device) {
    FaceDetect::Msg bob;
    bob.mFrame = mPipe.pop();
    mFaceRecognition->detect(bob);
    if (mPreviewCallback != nullptr) {
        mPreviewCallback(bob);
    }
		
	bob.mFrame.mRawdata.release();
#ifdef IR_CAMERA
	bob.mFrame.IR_mRawdata.release();
#endif
}

void FaceDetectService::faceRecognitionLoop(int device) {
    FaceDetect::Msg bob;
    bob.mMtcnnInterfaceOut = mPipe_frg.pop();
    bob.mFacenetDeviceCoreId = device;
    mFaceRecognition->facenetDetect(bob);
    if(mRecognitionCallback != nullptr) {
        mRecognitionCallback(bob);
    }
}


