#ifndef MODULE_H
#define MODULE_H

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string.h>

class DeviceInfo
{
public:
    int mDeviceID;
    std::string mBrief;
    DeviceInfo() : mDeviceID(-1), mBrief("null"){}
};

class UserInfo{
public:
    int mUserID;
    std::string mUserName;
    UserInfo() : mUserID(-1), mUserName("null"){}
};
typedef std::vector<UserInfo> UserInfoList;

class FaceInfo
{
public:
    int mUserID;
    float mFeatureMap[512];
    FaceInfo() : mUserID(-1){
        memset(mFeatureMap, 0, 512 * sizeof (float));
    }
    void getFeatureMap(float featureMap[512]) {
        memcpy(featureMap, mFeatureMap, sizeof (float) * 512);
    }

    void setFeatureMap(float featureMap[512]) {
        memcpy(mFeatureMap, featureMap, sizeof (float) * 512);
    }

    std::string toString() {
        std::stringstream ss;
        ss << "{"
           << mUserID << ","
           << mFeatureMap[0] << ","
           << mFeatureMap[1] << ","
           << mFeatureMap[2] << ","
           << mFeatureMap[3] << "}";
        return ss.str();
    }
};
typedef std::vector<FaceInfo> FaceInfoList;

class PermissionInfo
{
public:
    int mUserID;
    int mDeviceID;
    int mPermission;
    int mPrimaryKey;
    PermissionInfo() : mUserID(-1), mDeviceID(-1), mPermission(-1), mPrimaryKey(-1){}
};
typedef std::vector<PermissionInfo> PermissionInfoList;


class EventInfo
{
public:
    enum {
        InValid,
        OpenDoor,
        Attendance,
        OutAttendance
    };

    int mUserID;
    int mDeviceID;
    int mEventType;
    time_t mTimeStamp;

    EventInfo() : mUserID(-1), mDeviceID(-1), mEventType(InValid), mTimeStamp(time(nullptr)){}
};

#endif // MODULE_H
