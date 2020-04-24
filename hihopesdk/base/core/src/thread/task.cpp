#include "task.h"

Task::Task(LoopOnce func) :
    mLoopOnceFunc(func) {
}

void Task::start() {
    run();
}

void Task::stop() {
    if (isRunning()) {
        requestExitAndWait();
    }
}

bool Task::loopOnce() {
    return mLoopOnceFunc();
}
