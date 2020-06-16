#ifndef POOL_H
#define POOL_H

#include <array>
#include <list>
#include <mutex>
#include <iostream>
#include <condition_variable>

template<typename T>
class Pool {
public:
    T mObj;
    std::mutex mMutex;

    void set(T &obj) {
        std::lock_guard<std::mutex> locker(mMutex);
        mObj = obj;
    }

    void get(T &obj) {
        std::lock_guard<std::mutex> locker(mMutex);
        obj = mObj;
    }

//    T get() {
//        return mObj;
//    }
};

#endif // POOL_H
