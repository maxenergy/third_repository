#ifndef FACEATTENDANCE_H
#define FACEATTENDANCE_H

#include "facedetectservice.h"
#include "db_facerecognition.h"

class FaceAttendance
{
public:
    FaceAttendance();
    void userDetect(UserInfoList userList);
    void online(int userID, int deviceID);
    void offline(int userID, int deviceID);
    bool isAttendance();
};

#endif // FACEATTENDANCE_H
