#ifndef APP_H
#define APP_H

#include "core_public.h"
#include "db_facerecognition.h"
#include "facedetect.h"
#include "facerecognition.h"
#include "faceaccesscontrol.h"
#include "faceattendance.h"


class FaceRecognitionApi : public Singleton<FaceRecognitionApi>
{
public:
    enum {
        FaceAttendanceMode,
        FaceAccessControlModel
    };
    typedef std::function<void(FaceDetect::Msg bob)> PreviewCallback;
    int mModel;
	PreviewCallback mPreviewCallback;
	PreviewCallback mXdetectCallback;

    Camera *mCamera;
	Camera *mCamera_IR;
	Camera *mCamera_obj;
	std::mutex mMutex;
	std::list<UserInfo> mTmp_FaceDB;
	
    DBCache *mDBCache;
    FaceDetect *mFaceDetect;
    FaceRecognition *mFaceRecognition;
    FaceDetectService *mFaceDetectService;

    FaceAttendance *mFaceAttendance;
    FaceAccessControl *mFaceAccessControl;

    bool init();
    void setModel(int model);

    bool capture(VIFrame &photo);
	
	bool capture(int ch,VIFrame &photo);

    bool getUserInfo(int userID, UserInfo &info);
    bool updateUserInfo(UserInfo &info);

	bool delface(int userid);
	
    bool updateFaceInfo(FaceInfo &info);
    bool updateFaceInfo(int userID, VIFrame &facePhoto);
	bool updateFaceInfo(int userId, cv::Mat &facePhoto);

    bool getCurDevicePermissionInfo(int userID, PermissionInfo &info);
    bool getAllDevicePermissionInfo(int userID, std::map<int, PermissionInfo> &infos);
    bool updatePermissionInfo(PermissionInfo &info);

    void stopCameraPreview();
    void startCameraPreview();
    void setCameraPreviewCallBack(PreviewCallback func);
	void setXdetectCallBack(PreviewCallback func);

private:
    void handleFaceDetectServiceResult_detect(FaceDetect::Msg bob);
	void handleFaceDetectServiceResult_Reconition(FaceDetect::Msg bob);
	void handleFaceDetectServiceResult_xdetect(FaceDetect::Msg bob);

    Singleton_Decalartion(FaceRecognitionApi);
};

#endif // APP_H
