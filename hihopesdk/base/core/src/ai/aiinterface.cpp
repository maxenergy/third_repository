#include "aiinterface.h"


void AIInterface::setNetParam(int key, float value) {
    mNetParams[key] = value;
}

std::string AIInterface::getName(std::string vendor, std::string net) {
    return vendor + "_" + net;
}

AIInterface *AIInterface::getImpl(std::string vendor, std::string net) {
    std::string identify = getName(vendor, net);
    return ReflectObjectClass::createObject<AIInterface>(identify);
}
