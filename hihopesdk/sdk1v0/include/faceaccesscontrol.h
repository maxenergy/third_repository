#ifndef FACEACCESSCONTROL_H
#define FACEACCESSCONTROL_H

#include "facedetectservice.h"
#include "db_facerecognition.h"

class FaceAccessControl
{
public:
    FaceAccessControl();
    void userDetect(UserInfoList userList);

private:
    void handleDoor(int userID, int doorID);
    PermissionInfo getPermissionInfo(int userID, int doorID);
    bool openDoor(int userID, int doorID);
};

#endif // FACEACCESSCONTROL_H
