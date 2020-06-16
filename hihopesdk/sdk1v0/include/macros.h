#ifndef MACROS_H
#define MACROS_H

#ifndef CHECK_STATUS
#define CHECK_STATUS(RET, ERRSTR) if (mStatus != 0) { std::cout << "ERROR!!! " << ERRSTR << std::endl; return RET;}
#endif

#ifndef CHECK_STATUS_NOLOG
#define CHECK_STATUS_NOLOG(RET) if (mStatus != 0) { return RET;}
#endif

#ifndef CHECK_VAL
#define CHECK_VAL(VAL, RET) if (VAL != 0) { return RET;}
#endif

#ifndef CHECK_VAL_AND_LOG
#define CHECK_VAL_AND_LOG(VAL, RET, ERRSTR) if (VAL != 0) { std::cout << "ERROR!!! " << ERRSTR << std::endl; return RET;}
#endif

#endif // MACROS_H
