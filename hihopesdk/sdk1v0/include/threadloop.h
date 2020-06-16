#ifndef THREADLOOP_H
#define THREADLOOP_H

#include <mutex>
#include <atomic>
#include <thread>
#include <condition_variable>

class ThreadLoop
{
public:
    ThreadLoop();
    virtual ~ThreadLoop();

    int run();
    bool isRunning();
    bool exitPending();
    void requestExit();
    void requestExitAndWait();
    virtual bool readyToRun();

protected:
    virtual bool loopOnce() = 0;

private:
    pid_t mTid;
    //volatile bool mReady;
    //volatile bool mRunning;
    //volatile bool mExitPending;
    std::atomic_bool mReady;
    std::atomic_bool mRunning;
    std::atomic_bool mExitPending;
    std::mutex mThreadLoopMutex;
    std::condition_variable mThreadLoopCond;

    int loop();
};

#endif // THREADLOOP_H
