#ifndef DBCACHE_H
#define DBCACHE_H

#include "module.h"
#include <map>
#include <mutex>

class DBCache
{
public:
    DBCache();

    size_t mNumber;
    int *mUserIDs;
    float *mFeatureMaps;

    std::mutex mMutex;

    DeviceInfo mDeviceInfo;
    std::map<int, UserInfo> mUserInfoMaps;
    std::map<int, PermissionInfo> mPermissionMaps; //userID->PermissionInfo


    bool init();

    bool getUserInfo(int userID, UserInfo &info);
    bool updateUserInfo(UserInfo &info);

    bool updateFaceInfo(FaceInfo &info);

    bool getCurDevicePermissionInfo(int userID, PermissionInfo &info);
    bool getAllDevicePermissionInfo(int userID, PermissionInfoList &info);
    bool updatePermissionInfo(PermissionInfo &info);
	
	int filter(float *featureMap);

    UserInfo getUserInfoByFeatureMapsIdx(size_t idx);
};

#endif // DBCACHE_H
