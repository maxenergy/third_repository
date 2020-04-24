#ifndef OPERATORINTERFACE_H
#define OPERATORINTERFACE_H

#include "../reflect/reflectobject.h"

class OperatorInterface : public ReflectObject {
public:
    ~OperatorInterface() = default;
    virtual bool init() = 0;
    virtual int match(int *feature, int *featureList, int featureListSize, float *outDistance) = 0;
    virtual void preWhiten(unsigned char *input, unsigned int len, int *outBuf) = 0;
};

#endif // OPERATORINTERFACE_H
