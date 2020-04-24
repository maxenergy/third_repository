#include "operatorfactory.h"
#include "cpuoperator.h"

std::mutex OperatorFactory::mMutex;
std::map<std::string, OperatorInterface *> OperatorFactory::mOperatorImpl;

bool OperatorFactory::init() {
    CPUOperator op;
    op.init();
    return true;
}

OperatorInterface *OperatorFactory::createSingleInstance(std::string vendor) {
    std::lock_guard<std::mutex> locker(mMutex);
    auto found = mOperatorImpl.find(vendor);
    if (found != mOperatorImpl.end()) {
        return found->second;
    }

    OperatorInterface *impl = ReflectObjectClass::createObject<OperatorInterface>(vendor);
    if (impl->init()) {
        return impl;
    };
    return nullptr;
}

