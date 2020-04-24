#include "serverrequestmanager.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

ServerRequestManager::ServerRequestManager()
{

}

int ServerRequestManager::login(DeviceInfo &info) {
    return 0;
}

int ServerRequestManager::regist(DeviceInfo &info) {
    return 0;
}

int ServerRequestManager::getUserInfos(int deviceID, std::map<int, UserInfo> &infoMap) {
    return 0;
}

int ServerRequestManager::getFaceInfos(int deviceID, std::map<int, FaceInfo> &infoMap) {
    return 0;
}

int ServerRequestManager::getDoorPermissionInfos(int deviceID, std::map<int, PermissionInfo> &infoMap) {
    return 0;
}

int ServerRequestManager::postFaceInfo(int deviceID, FaceInfo &info) {
    return 0;
}

int ServerRequestManager::postEventInfo(int deviceID, EventInfo &info) {
    return 0;
}

int ServerRequestManager::postDoorPermissionInfo(int deviceID, PermissionInfo &info) {
    return 0;
}

#pragma GCC diagnostic pop
