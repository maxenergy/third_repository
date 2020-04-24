#include "threadloop.h"
#include "../utils/errors.h"
#include <unistd.h>
#include <sys/syscall.h>

ThreadLoop::ThreadLoop()
{
    mTid = -1;
    mReady = false;
    mRunning = false;
    mExitPending = false;
}

ThreadLoop::~ThreadLoop() {

}

int ThreadLoop::run() {
    std::lock_guard<std::mutex> locker(mThreadLoopMutex);
    if (mRunning) {
        return INVALID_OPERATION;
    }

    mTid = -1;
    mReady = false;
    mExitPending = false;

    std::thread t(&ThreadLoop::loop, this);
    t.detach();
    return NO_ERROR;
}

bool ThreadLoop::readyToRun() {
    return true;
}

void ThreadLoop::requestExit() {
    {
        std::unique_lock<std::mutex> locker(mThreadLoopMutex);
        mExitPending = true;
    }
}

void ThreadLoop::requestExitAndWait() {
    {
        std::unique_lock<std::mutex> locker(mThreadLoopMutex);
        mExitPending = true;
        mThreadLoopCond.wait(locker, [&](){return !mRunning;});
    }
}

bool ThreadLoop::exitPending() {
    {
        std::unique_lock<std::mutex> locker(mThreadLoopMutex);
        return mExitPending;
    }
}

bool ThreadLoop::isRunning() {
    {
        std::unique_lock<std::mutex> locker(mThreadLoopMutex);
        return mRunning;
    }
}

int ThreadLoop::loop() {
    mTid =  static_cast<pid_t>(syscall(SYS_gettid));
    if(readyToRun()){
        mRunning = true;
        bool result = true;
        while(result && !mExitPending) {
            result = loopOnce();
        }
    }

    {
        std::unique_lock<std::mutex> locker(mThreadLoopMutex);
        mTid = -1;
        mRunning = false;
        mThreadLoopCond.notify_all();
    }

    return NO_ERROR;
}
