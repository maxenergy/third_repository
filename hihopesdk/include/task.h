#ifndef TASK_H
#define TASK_H

#include "threadloop.h"
#include <functional>

class Task : public ThreadLoop
{
public:
    typedef std::function<bool()> LoopOnce;

    Task(LoopOnce func);

    virtual void stop();
    virtual void start();


private:
    virtual bool loopOnce();
    std::function<bool()> mLoopOnceFunc;
};

#endif // TASK_H
