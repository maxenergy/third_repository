#ifndef CPUOPERATOR_H
#define CPUOPERATOR_H

#include "operatorinterface.h"

class CPUOperator :  public OperatorInterface {
public:
    ~CPUOperator() = default;
    virtual bool init();
    virtual int match(int *feature, int *featureList, int featureListSize, float *outDistance);
    virtual void preWhiten(unsigned char *input, unsigned int len, int *outBuf);

private:
    ReflectObject_SpecRegister_DECLARATION(CPUOperator);
};

#endif // CPUOPERATOR_H
