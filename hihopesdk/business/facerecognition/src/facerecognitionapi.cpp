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
    mFaceDetectService->setPreviewCallBack(std::bind(&FaceRecognitionApi::handleFaceDetectServiceResult_detect, this, std::placeholders::_1));

	mFaceDetectService->setRecognitionCallBack(std::bind(&FaceRecognitionApi::handleFaceDetectServiceResult_Reconition, this, std::placeholders::_1));


	mFaceDetectService->setObjDetectCallBack(std::bind(&FaceRecognitionApi::handleFaceDetectServiceResult_xdetect, this, std::placeholders::_1));

    mFaceAttendance = new FaceAttendance();
    mFaceRecognition = new FaceRecognition();
    mFaceAccessControl = new FaceAccessControl();
    return true;
}

void FaceRecognitionApi::setModel(int model) {
    mModel = model;
}


void FaceRecognitionApi::handleFaceDetectServiceResult_xdetect(FaceDetect::Msg bob) { 
   if(mXdetectCallback != NULL)
    	mXdetectCallback(bob);
}



void FaceRecognitionApi::handleFaceDetectServiceResult_detect(FaceDetect::Msg bob) { 
	int keep_flag = 0;
    std::unique_lock<std::mutex> locker(mMutex);
	int db_size = mTmp_FaceDB.size();
   for(int i = 0; i<db_size; i++)
   {
   		keep_flag = 0;
		UserInfo item = mTmp_FaceDB.front();
		mTmp_FaceDB.pop_front();
		for(int j = 0; j< bob.mMtcnnInterfaceOut.mOutList.size();j++)
		{
			if(bob.mMtcnnInterfaceOut.mOutList[j].mTrackID == item.mTrackID)
			{
				keep_flag = 1;
				bob.mMtcnnInterfaceOut.mOutList[j].mUserID = item.mUserID;
				bob.mMtcnnInterfaceOut.mOutList[j].mUserName = item.mUserName;
				bob.mMtcnnInterfaceOut.mOutList[j].tracking_flag = 1;
			}
		}
		if(keep_flag){
			mTmp_FaceDB.push_back(item);
		}else{
			printf("remove track id %d \n",item.mTrackID);
		}
   }
   mFaceDetectService->trigger_frg(bob.mMtcnnInterfaceOut);
   if(mPreviewCallback != NULL)
    	mPreviewCallback(bob);
}

void FaceRecognitionApi::handleFaceDetectServiceResult_Reconition(FaceDetect::Msg bob) { 
#if 1	
    std::vector<UserInfo> users;
    mFaceRecognition->handleFaceRecognitionResult(bob, users);
	for (size_t i = 0; i < users.size(); i++) {
		UserInfo &info = users[i];
		if(info.mUserID != -1){
			 std::unique_lock<std::mutex> locker(mMutex);
			 mTmp_FaceDB.push_back(info);
			 printf("add track id %d TmpDb_size %d \n",info.mTrackID,mTmp_FaceDB.size());
		}
	}
#endif
}


bool FaceRecognitionApi::capture(VIFrame &photo) {
    return mCamera != nullptr && mCamera->isVaild() && mCamera->read(photo);
}

bool FaceRecognitionApi::capture(int ch,VIFrame &photo) {
    return mCamera != nullptr && mCamera->read(ch,photo);
}


bool FaceRecognitionApi::getUserInfo(int userID, UserInfo &info) {
    return mDBCache->getUserInfo(userID, info);
}

bool FaceRecognitionApi::updateUserInfo(UserInfo &info) {
    return mDBCache->updateUserInfo(info);
}


bool FaceRecognitionApi::delface(int userid){
	return mDBCache->delface(userid);
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

	if(msg.mMtcnnInterfaceOut.mOutList.size() > 1)
	{
		printf("More Than One face!\n");
		return false;
	}

	if(msg.mMtcnnInterfaceOut.mOutList.size()<1)
	{
		printf("not find face!\n");
		return false;
	}

	msg.mMtcnnInterfaceOut.mOutList[0].tracking_flag = 0x33;

    if (!mFaceDetect->facenetDetect(msg)) {
        std::cout << "facenetdetect failed in func FaceRecognitionApi::updateFaceInfo" << std::endl;
        return false;
    }


	if(msg.mFaceNetInterfaceOut.mOutList[0].mScore <= 0.5)
	{
		printf("bad face %f !\n",msg.mFaceNetInterfaceOut.mOutList[0].mScore);
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
    for (FaceNetOut item : msg.mFaceNetInterfaceOut.mOutList) {
        memcpy(info.mFeatureMap, item.mFeatureMap, 512 * sizeof (float));
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

void FaceRecognitionApi::setXdetectCallBack(PreviewCallback func){
	mXdetectCallback = func;
}

