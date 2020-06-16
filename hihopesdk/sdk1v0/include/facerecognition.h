#ifndef FACERECOGNITION_H
#define FACERECOGNITION_H

#include "facedetectservice.h"
#include "db_facerecognition.h"
#include <functional>

class FaceRecognitionApi;

class FaceRecognition
{
public:
    FaceRecognition();
    void handleFaceRecognitionResult(FaceDetect::Msg &bob, std::vector<UserInfo> &users);
    UserInfo filter(float *featureMap);

private:
    OperatorInterface *mOperator;
};

#endif // FACERECOGNITION_H
