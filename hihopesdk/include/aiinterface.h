#ifndef AIINTERFACE_H
#define AIINTERFACE_H

#include "frame.h"
#include "reflectobject.h"
#include "mtcnnout.h"
#include "facenetout.h"
#include "objectdetectout.h"
#include <vector>
#include <string>
#include <unordered_map>

enum {
    MTCNN_NMS_P,
    MTCNN_NMS_R,
    MTCNN_NMS_O,
    MTCNN_CONF_P,
    MTCNN_CONF_R,
    MTCNN_CONF_O,
    MTCNN_MINSIZE
};

class AIInterface : public ReflectObject {
public:
    virtual ~AIInterface() = default;
    virtual bool load(int device) = 0;
    virtual void setNetParam(int key, float value);

    static std::string getName(std::string vendor, std::string net);
    static AIInterface *getImpl(std::string vendor, std::string net);

protected:
    std::unordered_map<int, float> mNetParams;
};

template<typename AIINTERFACE, typename AIINTERFACEOUT>
class AIInterfaceBase : public AIInterface {
public:
    struct Out {
        std::vector<AIINTERFACEOUT> mOutList;
    };

    virtual bool detect(int device, Frame &frame, Out &out) = 0;
};


/*
template<typename AIINTERFACE>
class AIInterfaceRegister {
private:
    static typename ReflectObjectRegister<AIINTERFACE>::Register sRegister;
};

#define AIINTERFACE_REGISTER(CLASS_NAME, VENDOR, NET) \
    template<> \
    typename ReflectObjectRegister<CLASS_NAME>::Register AIInterfaceRegister<CLASS_NAME>::sRegister(AIInterface::getName(VENDOR, NET))
*/

#endif // AIINTERFACE_H
