#ifndef OPENCLMANAGER_H
#define OPENCLMANAGER_H

#include "core_public.h"

class OpenCLOperator : public OperatorInterface {
public:
    ~OpenCLOperator() = default;
    virtual bool init();
    virtual int match(int *feature, int *featureList, int featureListSize, float *outDistance);
    virtual void preWhiten(unsigned char *input, unsigned int len, int *outBuf);

private:
    ReflectObject_SpecRegister_DECLARATION(OpenCLOperator);

};

#endif // OPENCLMANAGER_H
