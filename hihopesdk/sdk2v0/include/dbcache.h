#ifndef DBCACHE_H
#define DBCACHE_H

#include "module.h"
#include <map>
#include <mutex>
#include "pola_sdk_export.h"

class DBCache
{
public:
    DBCache();

    size_t mNumber;
    int *mUserIDs;
    int *mFeatureMaps;

    std::mutex mMutex;

    DeviceInfo mDeviceInfo;
    std::map<int, UserInfo> mUserInfoMaps;
    std::map<int, PermissionInfo> mPermissionMaps; //userID->PermissionInfo

	Pola_Object_GP handle;

    bool init();
    bool clear();

    bool getUserInfo(int userID, UserInfo &info);
    bool updateUserInfo(UserInfo &info);

    int isFaceInfoExists(FaceInfo &info);
    bool updateFaceInfo(FaceInfo &info);


    bool getCurDevicePermissionInfo(int userID, PermissionInfo &info);
    bool getAllDevicePermissionInfo(int userID, std::map<int, PermissionInfo> &infos);
    bool updatePermissionInfo(PermissionInfo &info);
    UserInfo getUserInfoByFeatureMapsIdx(size_t idx);
	int filter(float *featureMap);
	
	bool delface(int userid);

private:
    bool creatTable();
    bool flushCache();
};

#endif // DBCACHE_H
