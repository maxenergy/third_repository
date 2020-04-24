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
    //mOperator->match(featureMap, mDBCache->mFeatureMaps, mDBCache->mNumber, &distance);
    
    //std::cout << mDBCache->mNumber << "----(" << idx  << "," << distance << ")" << std::endl;
    //if (distance > 0.85f) {
    //    return UserInfo();
    //}
	idx = mDBCache->filter(featureMap);

	//std::cout << "----(" << idx  << "," <<  ")--" << std::endl;
	if(idx == -1){
		userInfo.mUserID = -1;
		return userInfo;
	}
    return mDBCache->getUserInfoByFeatureMapsIdx(idx);
}


void FaceRecognition::handleFaceRecognitionResult(FaceDetect::Msg &bob, std::vector<UserInfo> &users) {
	//std::cout << "handleFaceRecognitionResult" << std::endl;
    //for (FaceNetOut item : bob.mFaceNetInterfaceOut.mOutList) {
#if 0
    for (MtcnnOut item : bob.mMtcnnInterfaceOut.mOutList) {
        UserInfo info = filter(item.faceinfo.mFeatureMap);
        if (info.mUserID != -1) {
            users.push_back(info);
			//item.mUserID = info.mUserID;
			//item.mUserName = info.mUserName;
        }
    }
#endif
	for (size_t i = 0; i < bob.mMtcnnInterfaceOut.mOutList.size(); i++) {
		MtcnnOut item = bob.mMtcnnInterfaceOut.mOutList[i];
		UserInfo info = filter(item.faceinfo.mFeatureMap);
		if (info.mUserID != -1) {
            users.push_back(info);
			bob.mMtcnnInterfaceOut.mOutList[i].mUserID = info.mUserID;
			bob.mMtcnnInterfaceOut.mOutList[i].mUserName = info.mUserName;
        }
		else{
			users.push_back(info);
			bob.mMtcnnInterfaceOut.mOutList[i].mUserID = -1;
			//bob.mMtcnnInterfaceOut.mOutList[i].mUserName = info.mUserName;
		}
	}

    for (size_t i = 0; i < users.size(); i++) {
        UserInfo &info = users[i];
        //bob.mMtcnnInterfaceOut.mOutList[i].mUserID = info.mUserID;
		//bob.mMtcnnInterfaceOut.mOutList[i].mUserName = info.mUserName;
		
        MtcnnOut mtcnnOut = bob.mMtcnnInterfaceOut.mOutList[i];
		
        
        std::cout << "user->{" << mtcnnOut.mUserID << "," << mtcnnOut.mUserName << "}" << std::endl;
		//std::cout << "user->{" << bob.mMtcnnInterfaceOut.mOutList[i].mUserID << "," << bob.mMtcnnInterfaceOut.mOutList[i].mUserName << "}" << std::endl;
		/*
        std::cout << "{";
        std::cout << mtcnnOut.mRect.tl().x << ",";
        std::cout << mtcnnOut.mRect.tl().y << ",";
        std::cout << mtcnnOut.mRect.br().x << ",";
        std::cout << mtcnnOut.mRect.br().y;
        std::cout << "}";

        std::cout << "{";
        std::cout << (mtcnnOut.mRect.tl()*3).x << ",";
        std::cout << (mtcnnOut.mRect.tl()*3).y << ",";
        std::cout << (mtcnnOut.mRect.br()*3).x << ",";
        std::cout << (mtcnnOut.mRect.br()*3).y << ",";
        std::cout << "}";
        std::cout << std::endl;
        */
        //cv::putText(bob.mFrame.mFrameData,
        //            cv::String(mtcnnOut.users.mUserName.c_str()),
        //            cvPoint(mtcnnOut.mRect.x*3, mtcnnOut.mRect.y*3-10),
        //            cv::FONT_HERSHEY_COMPLEX, 0.8, cv::Scalar(0, 0, 255), 1, 8, 0);
    }

}
