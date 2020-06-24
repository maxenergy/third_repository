#ifndef AISERVICEEXT_H
#define AISERVICEEXT_H

#include "aiservice.h"
#include "aiinterfaceext.h"

#define AISERVICE_DECLARATION(CLASS_NAME, AIINTERFACE_CLASS_NAME) \
    class CLASS_NAME : public AIService<AIINTERFACE_CLASS_NAME>{ \
    public: \
        CLASS_NAME(std::string vendor); \
    }; \


AISERVICE_DECLARATION(MtcnnService, MtcnnInterface)
AISERVICE_DECLARATION(FaceNetService, FaceNetInterface)
AISERVICE_DECLARATION(XDetectService, ObjectDetectInterface)

#endif // AISERVICEEXT_H
