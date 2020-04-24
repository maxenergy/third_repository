#include "facedetect.h"
#include <time.h>
#include <thread>
#include "dummy.h"
extern int init_face_rect(void);
extern void  close_dev(void);
FaceDetect::FaceDetect() {
	MtcnnDummyImpl();
#ifdef arm64
    mMtcnn = new MtcnnService("tengine");
    mFaceNets[0] = new FaceNetService("nnie");
    mFaceNets[1] = new FaceNetService("nnie");
#else
    mMtcnn = new MtcnnService("dummy");
    mFaceNets[0] = new FaceNetService("dummy");
    mFaceNets[1] = new FaceNetService("dummy");
#endif

    mFilterMaxFaceEnable = true;

    mCnt = 0;
    mCost = 0;
    mLastTime = time(nullptr);
}
FaceDetect::~FaceDetect()
{
	close_dev();
}

bool FaceDetect::init() {
    int ret = true;
    ret = ret && mMtcnn != nullptr && mMtcnn->load(0) && mMtcnn->isValid(0);
    ret = ret && mFaceNets[0] != nullptr && mFaceNets[0]->load(0) && mFaceNets[0]->isValid(0);
    ret = ret && mFaceNets[1] != nullptr && mFaceNets[1]->load(1) && mFaceNets[1]->isValid(1);

    if (ret) {
        std::cout << "FaceRecognitionNet init ok" << std::endl;
    } else {
        std::cout << "FaceRecognitionNet init failed" << std::endl;
    }
	init_face_rect();
    return ret;
}

bool FaceDetect::isValid() {
    return mMtcnn->isValid(0) && mFaceNets[0]->isValid(0) && mFaceNets[1]->isValid(1);
}

bool FaceDetect::detect(Msg &bob) {
    bool ret(false);
    if (bob.mFrame.mFrameData.empty()&&bob.mFrame.mRawdata.empty()) {
        std::cout << "frame is empty" << std::endl;
        return ret;
    }
	ret = mtcnnDetect(bob);
    if (mCnt >= 100) {
        mCurTime = time(nullptr);
        std::cout << "face detect fps: " << mCnt/(mCurTime-mLastTime) << std::endl;
        mCnt = 0;
        mCost = 0;
        mLastTime = mCurTime;
    }
    return ret;
}

bool FaceDetect::mtcnnDetect(Msg &bob) {
    bool ret(false);
    if (bob.mFrame.mFrameData.empty()&&bob.mFrame.mRawdata.empty()) {
        std::cout << "frame is empty int fun " << __FUNCTION__ << std::endl;
        return ret;
    }
    MtcnnInterface::Out mtcnnInterfaceOut;
    ret = mMtcnn->detect(0, bob.mFrame, mtcnnInterfaceOut);
    if (!ret) {
        std::cout << "mtcnn net detect failed int fun " << __FUNCTION__ << std::endl;
    }
	if (mtcnnInterfaceOut.mOutList.empty()) {
		return true;
	}

	for (MtcnnOut box : mtcnnInterfaceOut.mOutList) {
		bob.mMtcnnInterfaceOut.mOutList.push_back(box);
	 }
	
    return ret;
}

bool FaceDetect::facenetDetect(Msg &bob) {
    return true;
}

void FaceDetect::drawRectangle(Msg &bob) {
}

