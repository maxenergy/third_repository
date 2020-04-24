#include "faceaccesscontrol.h"
#include "facerecognitionapi.h"
#include <functional>

FaceAccessControl::FaceAccessControl() {

}

void FaceAccessControl::userDetect(UserInfoList userList) {
    if (userList.empty()) {
        return;
    }

    DBCache *mDBCache = FaceRecognitionApi::getInstance().mDBCache;
    int deviceID = mDBCache->mDeviceInfo.mDeviceID;
    for (UserInfo user : userList) {
        handleDoor(user.mUserID, deviceID);
    }
}

void FaceAccessControl::handleDoor(int userID, int deviceID) {
    DBCache *mDBCache = FaceRecognitionApi::getInstance().mDBCache;
    PermissionInfo info;
    info.mUserID = userID;
    info.mDeviceID = deviceID;
    mDBCache->getCurDevicePermissionInfo(userID, info);
    if (info.mPermission) {
        openDoor(userID, deviceID);
    }
}

bool FaceAccessControl::openDoor(int userID, int deviceID) {
    EventInfo info;
    info.mUserID = userID;
    info.mDeviceID = deviceID;
    info.mEventType = EventInfo::OpenDoor;
    ServerRequestManager::postEventInfo(deviceID, info);
    return true;
}
