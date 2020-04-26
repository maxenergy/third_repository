#include "base/Mutex.h"
#include "base/New.h"

Mutex* Mutex::createNew()
{
#ifndef CUSTOM_NEW
    return new Mutex();
#else
    return New<Mutex>::allocate();
#endif
}

Mutex::Mutex()
{
    pthread_mutex_init(&mMutex, NULL);
}

Mutex::~Mutex()
{
    pthread_mutex_destroy(&mMutex);
}

void Mutex::lock()
{
    pthread_mutex_lock(&mMutex);
}

void Mutex::unlock()
{
    pthread_mutex_unlock(&mMutex);
}

MutexLockGuard::MutexLockGuard(Mutex* mutex) :
    mMutex(mutex)
{
    mMutex->lock();
}

MutexLockGuard::~MutexLockGuard()
{
    mMutex->unlock();
}
