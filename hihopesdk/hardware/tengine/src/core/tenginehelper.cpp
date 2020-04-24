#include <iostream>
#include "tenginehelper.h"
#include "tengine_c_api.h"

static std::mutex sMutex;
static int sTengineStatus;


int tengineInit() {
    std::lock_guard<std::mutex> locker(sMutex);
    if(sTengineStatus == 1) {
        return 0;
    }

    if(init_tengine() < 0) {
       std::cout << "init tengine failed\n";
       sTengineStatus = 0;
       return -1;
    }

    if(request_tengine_version("0.9") < 0) {
       std::cout << "request tengine version failed\n";
       sTengineStatus = 0;
       return -1;
    }

    sTengineStatus = 1;
    return 0;
}

int tengineRelease() {
    std::lock_guard<std::mutex> locker(sMutex);
    if(sTengineStatus == 0) {
        return 0;
    }
    release_tengine();
    sTengineStatus = 0;
    return 0;
}
