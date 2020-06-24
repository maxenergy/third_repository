#include "aiserviceext.h"

#define AISERVICE_IMPL(CLASS_NAME, NET_NAME) \
    CLASS_NAME::CLASS_NAME(std::string vendor) : AIService(vendor, NET_NAME) {}

AISERVICE_IMPL(MtcnnService, "mtcnn");
AISERVICE_IMPL(FaceNetService, "facenet");
AISERVICE_IMPL(XDetectService, "objectdetect");

