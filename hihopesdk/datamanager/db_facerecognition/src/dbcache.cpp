#include "dbcache.h"
#include "SQLiteCpp/Database.h"
#include "SQLiteCpp/Transaction.h"
#include <iostream>
#include <sstream>

static SQLite::Database db("facerecognition.db", SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE);

DBCache::DBCache()
{
    mNumber = 0;
    mUserIDs = nullptr;
    mFeatureMaps = nullptr;
    mDeviceInfo.mDeviceID = 100;
}

bool DBCache::creatTable() {
    try {
		
        db.exec("CREATE TABLE if not exists userinfo (id INTEGER PRIMARY KEY, name TEXT)");
        db.exec("CREATE TABLE if not exists faceinfo (id INTEGER PRIMARY KEY, feature_map blob)");
        db.exec("CREATE TABLE if not exists permissioninfo (id INTEGER PRIMARY KEY, user_id INTEGER, device_id INTEGER, permission INTEGER)");
    } catch (std::exception&) {
        std::cout << "sqlit error: " << db.getErrorMsg() << " in func DBCache::creatTable" << std::endl;
        return false;
    }
    return true;
}

bool DBCache::flushCache() {
    try {
        SQLite::Statement userQuery(db, "SELECT * FROM userinfo");
        while (userQuery.executeStep()) {
            UserInfo item;
            item.mUserID = userQuery.getColumn(0).getInt();
            item.mUserName = userQuery.getColumn(1).getText();
            mUserInfoMaps[item.mUserID] = item;
        }
        userQuery.reset();
        std::cout << "userinfo load ok" << std::endl;

        SQLite::Statement permissionQuery(db, "SELECT * FROM permissioninfo WHERE device_id = :cur_device");
        permissionQuery.bind(":cur_device", mDeviceInfo.mDeviceID);
        while (permissionQuery.executeStep()) {
            PermissionInfo item;
            item.mUserID = permissionQuery.getColumn(1).getInt();
            item.mDeviceID = permissionQuery.getColumn(2).getInt();
            item.mPermission = permissionQuery.getColumn(3).getInt();
            mPermissionMaps[item.mUserID] = item;
        }
        permissionQuery.reset();
        std::cout << "permissioninfo load ok" << std::endl;

//		FaceGroupHandle *handle;
//		FaceRetCode ret = createFaceGroup(handle);

        FaceInfoList faceInfoList;
        SQLite::Statement faceQuery(db, "SELECT * FROM faceinfo");
        while (faceQuery.executeStep()) {
            FaceInfo item;
            item.mUserID = faceQuery.getColumn(0).getInt();
            SQLite::Column colBlob = faceQuery.getColumn(1);
            const float *blob = static_cast<const float *>(colBlob.getBlob());
//			insertFaceGroup(handle,,);
            item.setFeatureMap(const_cast<float *>(blob));
            faceInfoList.push_back(item);
        }
        faceQuery.reset();

		ERROR_CODE ret_code = makeFaceGp(&handle);
		int feature_base_size = faceInfoList.size();
		
#if 1
        mNumber = faceInfoList.size();
        if (mNumber > 0) {
            mUserIDs = static_cast<int *>(malloc(mNumber * sizeof(int)));
            //mFeatureMaps = static_cast<int *>(malloc(mNumber * 512 * sizeof(float)));
            for (size_t i = 0; i < mNumber; i++) {
                mUserIDs[i] = faceInfoList[i].mUserID;
                ret_code = GpinsertItem(handle, faceInfoList[i].mFeatureMap, 512, (uint64_t)faceInfoList[i].mUserID);
            }
        }
        std::cout << "faceinfo load ok" << std::endl;
#endif

    } catch (std::exception&) {
        std::cout << "sqlit error: " << db.getErrorMsg() << " in func DBCache::flushCache" << std::endl;
        return false;
    }
    return true;
}

bool DBCache::init() {
    std::cout << "DBCache init" << std::endl;
    return creatTable() && flushCache();
}

bool DBCache::clear() {
    int ret;
    ret = db.exec("DROP TABLE IF EXISTS userinfo");
    ret = db.exec("DROP TABLE IF EXISTS faceinfo");
    ret = db.exec("DROP TABLE IF EXISTS permissioninfo");
	freeGp(handle);
    return ret;
}

bool DBCache::getUserInfo(int userID, UserInfo &info) {
	std::cout << "getUserInfo" << userID << std::endl;
    if (userID == -1) {
        std::cout << "invalid user id in func DBCache::getUserInfo" << std::endl;
        return false;
    }

    auto found = mUserInfoMaps.find(userID);
    if (found == mUserInfoMaps.end()) {
        std::cout << "not found in func DBCache::getUserInfo" << std::endl;
        return false;
    }
    info = found->second;
    if (info.mUserID != userID) {
        std::cout << "userid not match in func DBCache::getUserInfo" << std::endl;
        return false;
    }
    return true;
}

bool DBCache::updateUserInfo(UserInfo &info) {
    int userId = info.mUserID;
    if (userId == -1) {
        std::cout << "invalid user id in func DBCache::updateUserInfo" << std::endl;
        return false;
    }

    try {
        SQLite::Statement query(db, "REPLACE INTO userinfo(id, name) VALUES (?,?)");
        query.bind(1, info.mUserID);
        query.bind(2, info.mUserName);
        query.exec();
    } catch (std::exception&) {
        std::cout << "sqlit error: " << db.getErrorMsg() << " in func DBCache::updateUserInfo" << std::endl;
        return false;
    }
    mUserInfoMaps[userId] = info;
    return true;
}

int DBCache::isFaceInfoExists(FaceInfo &info) {
    if (info.mUserID == -1) {
        std::cout << "invalid user id in func DBCache::isFaceInfoExists" << std::endl;
        return false;
    }
    for (size_t i = 0; i < mNumber; i++) {
        if (mUserIDs[i] == info.mUserID) {
            //return static_cast<int>(i);
            return mUserIDs[i];
        }
    }
    return -1;
}

bool DBCache::updateFaceInfo(FaceInfo &info) {
    if (info.mUserID == -1) {
        std::cout << "invaild user id in func DBCache::updateFaceInfo" << std::endl;
        return false;
    }

    float blob[512];
    info.getFeatureMap(blob);
    try {
        SQLite::Statement query(db, "REPLACE INTO faceinfo(id, feature_map) VALUES (?,?)");
        query.bind(1, info.mUserID);
        query.bind(2, blob, 512*sizeof(float));
        query.exec();
    } catch (std::exception&) {
        std::cout << "sqlit error: " << db.getErrorMsg() << " in func DBCache::updateFaceInfo" << std::endl;
        return false;
    }

#if 1
    int found = isFaceInfoExists(info);
    if (found >= 0) {
		ERROR_CODE ret_code = GpdelItem(handle, found);
		ret_code = GpinsertItem(handle, blob, 512, (uint64_t)found);
        //memcpy(mFeatureMaps + found * 128, info.mFeatureMap, 512 * sizeof(float));
        std::cout << "replace face info" << ret_code << std::endl;
        return true;
    } else {
        mNumber++;
        mUserIDs = static_cast<int *>(realloc(mUserIDs, mNumber * sizeof(int)));
        //mFeatureMaps = static_cast<int *>(realloc(mFeatureMaps, mNumber * 512 * sizeof(float)));
        mUserIDs[mNumber-1] = info.mUserID;
		ERROR_CODE ret_code = GpinsertItem(handle, blob, 512, (uint64_t)info.mUserID);
        //memcpy(mFeatureMaps + (mNumber - 1)*512, info.mFeatureMap,  512 * sizeof(float));
        std::cout << "insert face info" << info.mUserID << std::endl;
        return true;
    }
#endif
}

bool DBCache::getCurDevicePermissionInfo(int userID, PermissionInfo &info) {
    if (userID == -1) {
        //std::cout << "invaild user id in func DBCache::getCurDevicePermissionInfo" << std::endl;
        return false;
    }

    auto found = mPermissionMaps.find(userID);
    if (found == mPermissionMaps.end()) {
        return false;
    }
    info = found->second;
    if (info.mUserID != userID) {
        return false;
    }
    return true;
}


bool DBCache::getAllDevicePermissionInfo(int userID, std::map<int, PermissionInfo> &infos) {
    if (userID == -1) {
        std::cout << "invaild user id in func DBCache::getAllDevicePermissionInfo" << std::endl;
        return false;
    }
    try {
        SQLite::Statement query(db, "SELECT * FROM permissioninfo WHERE user_id = ?");
        query.bind(1, userID);
        while (query.executeStep()) {
            PermissionInfo item;
            item.mUserID = query.getColumn(1).getInt();
            item.mDeviceID = query.getColumn(2).getInt();
            item.mPermission = query.getColumn(3).getInt();
            infos[item.mDeviceID] = item;
        }
        query.reset();
    } catch (std::exception exp) {
        std::cout << "sqlit error: " << db.getErrorMsg() << " in func DBCache::getAllDevicePermissionInfo" << std::endl;
        return false;
    }
    return true;
}

bool DBCache::updatePermissionInfo(PermissionInfo &info) {
    if (info.mUserID == -1) {
        std::cout << "invaild user id in func DBCache::updatePermissionInfo" << std::endl;
        return false;
    }
    if (info.mDeviceID == -1) {
        std::cout << "invaild device id in func DBCache::updatePermissionInfo" << std::endl;
        return false;
    }

    try {
        SQLite::Statement query(db, "INSERT INTO permissioninfo(user_id, device_id, permission) SELECT :user,:device,:perm WHERE NOT EXISTS(SELECT * FROM permissioninfo WHERE user_id=:user AND device_id=:device)");
        query.bind(":user", info.mUserID);
        query.bind(":device", info.mDeviceID);
        query.bind(":perm", info.mPermission);
        query.exec();
        query.reset();

        SQLite::Statement query1(db, "UPDATE permissioninfo SET permission = :perm WHERE user_id=:user AND device_id=:device");
        query1.bind(":perm", info.mPermission);
        query1.bind(":user", info.mUserID);
        query1.bind(":device", info.mDeviceID);
        query1.exec();
        query1.reset();
    } catch (std::exception exp) {
        std::cout << "sqlit error: " << db.getErrorMsg() << " in func DBCache::getAllDevicePermissionInfo" << std::endl;
        return false;
    }

    if (info.mDeviceID == mDeviceInfo.mDeviceID) {
        mPermissionMaps[info.mUserID] = info;
    }

    return true;
}

UserInfo DBCache::getUserInfoByFeatureMapsIdx(size_t idx) {
    UserInfo userInfo;
    std::cout << "invaild idx in func DBCache::getUserInfoByFeatureMapsIdx "<< idx << std::endl;
    getUserInfo(idx, userInfo);
    return userInfo;
}

int DBCache::filter(float *featureMap)
{
	uint64_t face_id = 0;
	float result;
	size_t size;
	ERROR_CODE ret_code = getGPsize(handle, &size);
	if(size != 0){
		ret_code = GpqueryItem(handle, featureMap, 512, &result, &face_id);
		std::cout << "filter result " << result << " face_id " << face_id << std::endl;
		if(result>60)
			return face_id;
		else
			return -1;
	}
	return -1;
}

