#include "cpuoperator.h"

ReflectObject_SpecRegister_IMPLEMENT(CPUOperator, "cpu_operator");

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

bool CPUOperator::init() {
    return true;
}

int CPUOperator::match(int *feature, int *featureList, int featureListSize, float *) {
    return 0;
}

void CPUOperator::preWhiten(unsigned char *input, unsigned int len, int *outBuf) {
}

#pragma GCC diagnostic pop
