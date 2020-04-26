#include <stdio.h>

#include "net/UsageEnvironment.h"
#include "base/New.h"

UsageEnvironment* UsageEnvironment::createNew(EventScheduler* scheduler, ThreadPool* threadPool)
{
    if(!scheduler)
        return NULL;

#ifndef CUSTOM_NEW
    return new UsageEnvironment(scheduler, threadPool);
#else
    return New<UsageEnvironment>::allocate(scheduler, threadPool);
#endif
}

UsageEnvironment::UsageEnvironment(EventScheduler* scheduler, ThreadPool* threadPool) :
    mScheduler(scheduler),
    mThreadPool(threadPool)
{

}

UsageEnvironment::~UsageEnvironment()
{

}

EventScheduler* UsageEnvironment::scheduler()
{
    return mScheduler;
}

ThreadPool* UsageEnvironment::threadPool()
{
    return mThreadPool;
}
