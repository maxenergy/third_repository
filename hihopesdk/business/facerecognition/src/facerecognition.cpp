#include "facerecognition.h"
#include "facerecognitionapi.h"
//#include "opencl_public.h"

FaceRecognition::FaceRecognition() {
#ifdef arm64
    mOperator = OperatorFactory::createSingleInstance("opencl_operator");
#else
    mOperator = OperatorFactory::createSingleInstance("cpu_operator");
#endif
}

UserInfo FaceRecognition::filter(float *featureMap) {
    DBCache *mDBCache = FaceRecognitionApi::getInstance().mDBCache;
    float distance;
    int idx = 0;
	UserInfo userInfo;
	idx = mDBCache->filter(featureMap);
	if(idx == -1){
		userInfo.mUserID = -2;
		return userInfo;
	}
    return mDBCache->getUserInfoByFeatureMapsIdx(idx);
}

void FaceRecognition::handleFaceRecognitionResult(FaceDetect::Msg &bob, std::vector<UserInfo> &users) {
	for (size_t i = 0; i < bob.mFaceNetInterfaceOut.mOutList.size(); i++) {
		FaceNetOut item = bob.mFaceNetInterfaceOut.mOutList[i];
		if((item.tracking_flag != 1) && (item.mScore > 0.5)) {
			UserInfo info = filter(item.mFeatureMap);
			info.mTrackID = item.mTrackID;
			users.push_back(info);
		}
	}

	for (size_t i = 0; i < users.size(); i++) {
		UserInfo &info = users[i];
		std::cout << "user->{" << info.mUserID << "," << info.mUserName << "}" << std::endl;
	}

}
