#ifndef SERVERREQUESTMANAGER_H
#define SERVERREQUESTMANAGER_H

#include "module.h"
#include <map>

class ServerRequestManager {
public:
    enum RetCode {
        SUCCESS,
        NO_NETWORK,
        NO_REGIST,
        NO_LOGIN
    };

    ServerRequestManager();
    static int login(DeviceInfo &info);
    static int regist(DeviceInfo &info);
    static int getUserInfos(int deviceID, std::map<int, UserInfo> &infoMap);
    static int getFaceInfos(int deviceID, std::map<int, FaceInfo> &infoMap);
    static int getDoorPermissionInfos(int deviceID, std::map<int, PermissionInfo> &infoMap);
    static int postFaceInfo(int deviceID, FaceInfo &info);
    static int postEventInfo(int deviceID, EventInfo &info);
    static int postDoorPermissionInfo(int deviceID, PermissionInfo &info);
};

#endif // SERVERREQUESTMANAGER_H
