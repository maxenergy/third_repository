#include <stdio.h>

#include "net/Event.h"
#include "base/New.h"

TriggerEvent* TriggerEvent::createNew(void* arg)
{
#ifndef CUSTOM_NEW
    return new TriggerEvent(arg);
#else
    return New<TriggerEvent>::allocate(arg);
#endif
}

TriggerEvent* TriggerEvent::createNew()
{
#ifndef CUSTOM_NEW
    return new TriggerEvent(NULL);
#else
    return New<TriggerEvent>::allocate((void*)0);
#endif
}

TriggerEvent::TriggerEvent(void* arg) :
    mArg(arg)
{

}

void TriggerEvent::handleEvent()
{
    if(mTriggerCallback)
        mTriggerCallback(mArg);
}

TimerEvent* TimerEvent::createNew(void* arg)
{
#ifndef CUSTOM_NEW
    return new TimerEvent(arg);
#else
    return New<TimerEvent>::allocate(arg);
#endif
}

TimerEvent* TimerEvent::createNew()
{
#ifndef CUSTOM_NEW
    return new TimerEvent(NULL);
#else
    return New<TimerEvent>::allocate((void*)0);
#endif
}

TimerEvent::TimerEvent(void* arg) :
    mArg(arg)
{
    
}

void TimerEvent::handleEvent()
{
    if(mTimeoutCallback)
        mTimeoutCallback(mArg);
}

IOEvent* IOEvent::createNew(int fd, void* arg)
{
    if(fd < 0)
        return NULL;
#ifndef CUSTOM_NEW
    return new IOEvent(fd, arg);
#else
    return New<IOEvent>::allocate(fd, arg);
#endif
}

IOEvent* IOEvent::createNew(int fd)
{
    if(fd < 0)
        return NULL;
#ifndef CUSTOM_NEW
    return new IOEvent(fd, NULL);
#else
    return New<IOEvent>::allocate(fd, (void*)0);
#endif
}

IOEvent::IOEvent(int fd, void* arg) :
    mFd(fd),
    mArg(arg),
    mEvent(EVENT_NONE),
    mREvent(EVENT_NONE),
    mReadCallback(NULL),
    mWriteCallback(NULL),
    mErrorCallback(NULL)
{

}

void IOEvent::handleEvent()
{
    if (mReadCallback && (mREvent & EVENT_READ))
    {
        mReadCallback(mArg);
    }

    if (mWriteCallback && (mREvent & EVENT_WRITE))
    {
        mWriteCallback(mArg);
    }
    
    if (mErrorCallback && (mREvent & EVENT_ERROR))
    {
        mErrorCallback(mArg);
    }
};
