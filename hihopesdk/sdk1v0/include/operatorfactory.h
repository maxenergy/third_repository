#ifndef OPERATORMANAGER_H
#define OPERATORMANAGER_H

#include "operatorinterface.h"
#include <map>
#include <mutex>

class OperatorFactory {
public:
    static bool init();
    static OperatorInterface *createSingleInstance(std::string vendor);

private:
    static std::mutex mMutex;
    static std::map<std::string, OperatorInterface *> mOperatorImpl;
};

#endif // OPERATORMANAGER_H
