#include "opencloperator.h"

void opencl_init(void);
void opencl_prewhiten(char *Input,int len,int *out_buf);
int MatchFeature(int *idxOut, float *outDistance, int * face, int *face_lib, int lib_size);

ReflectObject_SpecRegister_IMPLEMENT(OpenCLOperator, "opencl_operator");

bool OpenCLOperator::init() {
    opencl_init();
    return true;
}

int OpenCLOperator::match(int *feature, int *featureList, int featureListSize, float *outDistance) {
    int idxOut;
    MatchFeature(&idxOut, outDistance, feature, featureList, featureListSize);
    return idxOut;
}

void OpenCLOperator::preWhiten(unsigned char *input, unsigned int len, int *outBuf) {
    opencl_prewhiten((char *)input, (unsigned int)len, outBuf);
}

