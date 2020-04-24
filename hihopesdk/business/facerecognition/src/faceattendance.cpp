#include "faceattendance.h"
#include "facerecognitionapi.h"

FaceAttendance::FaceAttendance() {

}

void FaceAttendance::userDetect(UserInfoList userList) {
    if (userList.empty()) {
        return;
    }
    DBCache *mDBCache = FaceRecognitionApi::getInstance().mDBCache;
    int deviceID = mDBCache->mDeviceInfo.mDeviceID;

    if (isAttendance()) {
        for (UserInfo user : userList) {
            online(user.mUserID, deviceID);
        }
    } else {
        for (UserInfo user : userList) {
            offline(user.mUserID, deviceID);
        }
    }
}

void FaceAttendance::online(int userID, int deviceID) {
    EventInfo info;
    info.mUserID = userID;
    info.mDeviceID = deviceID;
    info.mEventType = EventInfo::Attendance;
    info.mTimeStamp = time(nullptr);
    ServerRequestManager::postEventInfo(deviceID, info);

}
void FaceAttendance::offline(int userID, int deviceID) {
    EventInfo info;
    info.mUserID = userID;
    info.mDeviceID = deviceID;
    info.mEventType = EventInfo::OutAttendance;
    info.mTimeStamp = time(nullptr);
    ServerRequestManager::postEventInfo(deviceID, info);
}

bool FaceAttendance::isAttendance() {
    //time_t curTime = time(nullptr);
    return true;
}
