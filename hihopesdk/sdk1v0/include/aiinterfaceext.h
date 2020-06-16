#ifndef AIINTERFACEEXT_H
#define AIINTERFACEEXT_H

#include "aiinterface.h"

#define AIINTERFACE_DECLARATION(CLASS_NAME, OUT_CLASS_NAME) \
    class CLASS_NAME : public AIInterfaceBase<CLASS_NAME, OUT_CLASS_NAME> { \
    };\

AIINTERFACE_DECLARATION(MtcnnInterface, MtcnnOut);
AIINTERFACE_DECLARATION(FaceNetInterface, FaceNetOut);
AIINTERFACE_DECLARATION(ObjectDetectInterface, ObjectDetectOut);


#endif // AIINTERFACEEXT_H
