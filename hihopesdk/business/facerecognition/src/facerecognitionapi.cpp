#include "facerecognitionapi.h"

#define FaceInfoFile "faceinfo.db"
#define SqliteDBFile "facerecogintion_app.db"

FaceRecognitionApi::FaceRecognitionApi() {
}

bool FaceRecognitionApi::init() {
    mDBCache = new DBCache();
    if (!mDBCache->init()) {
        return false;
    }

#ifdef arm64
    mCamera = new Camera("MipiCamera");
#else
#ifdef arm
    mCamera = new Camera("MipiCamera");

#ifdef IR_CAMERA
	mCamera_IR = new Camera("MipiCamera");
#endif

#else
    mCamera = new Camera("CVCamera");
#endif
#endif
    if (!mCamera->open(0)) {
        return false;
    }
	
#ifdef IR_CAMERA
	if (!mCamera_IR->open(2)) {
        return false;
    }
#endif

    mFaceDetect = new FaceDetect();
    if (!mFaceDetect->init()) {
        return false;
    }
    mFaceDetectService = new FaceDetectService(mFaceDetect);
    mFaceDetectService->mCamera = mCamera;

#ifdef IR_CAMERA
	mFaceDetectService->mCamera_IR = mCamera_IR;
#endif

    mFaceDetectService->setPreviewCallBack(std::bind(&FaceRecognitionApi::handleFaceDetectServiceResult, this, std::placeholders::_1));

    mFaceAttendance = new FaceAttendance();
    mFaceRecognition = new FaceRecognition();
    mFaceAccessControl = new FaceAccessControl();
    return true;
}

void FaceRecognitionApi::setModel(int model) {
    mModel = model;
}

void FaceRecognitionApi::handleFaceDetectServiceResult(FaceDetect::Msg bob) {
    //mPreviewCallback(bob.mFrame.mFrameData);
    
    std::vector<UserInfo> users;
    mFaceRecognition->handleFaceRecognitionResult(bob, users);
	mPreviewCallback(bob);

//    if (mModel == FaceAttendanceMode) {
//        mFaceAttendance->userDetect(users);
//    }
//    if (mModel == FaceAccessControlModel) {
//        mFaceAccessControl->userDetect(users);
//    }

//    for (UserInfo user : users) {
//        std::cout << user.mUserName << std::endl;
//    }
}


bool FaceRecognitionApi::capture(VIFrame &photo) {
    return mCamera != nullptr && mCamera->isVaild() && mCamera->read(photo);
}

bool FaceRecognitionApi::getUserInfo(int userID, UserInfo &info) {
    return mDBCache->getUserInfo(userID, info);
}

bool FaceRecognitionApi::updateUserInfo(UserInfo &info) {
    return mDBCache->updateUserInfo(info);
}

bool FaceRecognitionApi::updateFaceInfo(FaceInfo &info) {
    return mDBCache->updateFaceInfo(info);
}

bool FaceRecognitionApi::updateFaceInfo(int userId, VIFrame &facePhoto) {
    if (userId == -1) {
        std::cout << "invalid userid in func FaceRecognitionApi::updateFaceInfo" << std::endl;
        return false;
    }
    if (facePhoto.empty()) {
        std::cout << "invalid face photo in func FaceRecognitionApi::updateFaceInfo" << std::endl;
        return false;
    }

    FaceDetect::Msg msg;
    msg.mFrame.mRawdata = facePhoto;
    msg.mFacenetDeviceCoreId = 0;
    if (!mFaceDetect->detect(msg)) {
        std::cout << "detect failed in func FaceRecognitionApi::updateFaceInfo" << std::endl;
        return false;
    }
	#if 0
    if (msg.mFaceNetInterfaceOut.mOutList.empty()) {
        std::cout << "detect nothing in func FaceRecognitionApi::updateFaceInfo" << std::endl;
        return false;
    }
	#endif
    FaceInfo info;
    info.mUserID = userId;
    for (MtcnnOut item : msg.mMtcnnInterfaceOut.mOutList) {
        memcpy(info.mFeatureMap, item.faceinfo.mFeatureMap, 512 * sizeof (float));
    }
	//msg.mMtcnnInterfaceOut.mOutList.clear();
    return updateFaceInfo(info);
}


bool FaceRecognitionApi::updateFaceInfo(int userId, cv::Mat &facePhoto) {
    if (userId == -1) {
        std::cout << "invalid userid in func FaceRecognitionApi::updateFaceInfo" << std::endl;
        return false;
    }
    if (facePhoto.empty()) {
        std::cout << "invalid face photo in func FaceRecognitionApi::updateFaceInfo" << std::endl;
        return false;
    }

    FaceDetect::Msg msg;
    msg.mFrame.mFrameData = facePhoto;
    msg.mFacenetDeviceCoreId = 0;
    if (!mFaceDetect->detect(msg)) {
        std::cout << "detect failed in func FaceRecognitionApi::updateFaceInfo" << std::endl;
        return false;
    }
	#if 0
    if (msg.mFaceNetInterfaceOut.mOutList.empty()) {
        std::cout << "detect nothing in func FaceRecognitionApi::updateFaceInfo" << std::endl;
        return false;
    }
	#endif
    FaceInfo info;
    info.mUserID = userId;
    for (MtcnnOut item : msg.mMtcnnInterfaceOut.mOutList) {
        memcpy(info.mFeatureMap, item.faceinfo.mFeatureMap, 512 * sizeof (float));
    }
	//msg.mMtcnnInterfaceOut.mOutList.clear();
    return updateFaceInfo(info);
}


bool FaceRecognitionApi::getCurDevicePermissionInfo(int userID, PermissionInfo &info) {
    return mDBCache->getCurDevicePermissionInfo(userID, info);
}

bool FaceRecognitionApi::getAllDevicePermissionInfo(int userID, std::map<int, PermissionInfo> &infos) {
    return mDBCache->getAllDevicePermissionInfo(userID, infos);
}

bool FaceRecognitionApi::updatePermissionInfo(PermissionInfo &info) {
    return mDBCache->updatePermissionInfo(info);
}

void FaceRecognitionApi::stopCameraPreview() {
    mFaceDetectService->stop();
}

void FaceRecognitionApi::startCameraPreview() {
    mFaceDetectService->start();
}

void FaceRecognitionApi::setCameraPreviewCallBack(PreviewCallback func) {
    mPreviewCallback = func;
}
