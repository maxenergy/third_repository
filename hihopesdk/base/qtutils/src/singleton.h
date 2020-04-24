#ifndef SINGLETON_H
#define SINGLETON_H

#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <mutex>
#include <memory>


template <typename T>
class Singleton
{
public:
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    static T& getInstance() {
        static std::once_flag s_once_flag;
        std::call_once(s_once_flag, [&]{
            sInstance.reset(new T());
        });
        return *sInstance;
    }

protected:
    Singleton() { }
    ~Singleton() { }

private:
    static std::unique_ptr<T> sInstance;
};

template <typename T> std::unique_ptr<T> Singleton<T>::sInstance;

#endif // SINGLETON_H
